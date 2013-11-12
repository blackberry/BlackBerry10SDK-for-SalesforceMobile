/*
 * SFRestAPI.cpp
 *
 *  Created on: Oct 18, 2013
 *      Author: Livan Yi Du
 */

#include "SFRestAPI.h"
#include <bb/cascades/WebView>
#include <bb/cascades/WebSettings>
#include <bb/device/HardwareInfo>
#include <bb/platform/PlatformInfo>
#include <bb/Application>
#include <QtNetwork/QNetworkRequest>
#include <QTextStream>
#include "SFGlobal.h"
#include "SFRestRequest.h"
#include "SFRestResourceTask.h"
#include "SFAuthenticationManager.h"
#include "SFOAuthCredentials.h"
#include "SFAccountManager.h"
#include "SFOAuthCoordinator.h"
namespace sf {

SFRestAPI *SFRestAPI::sharedInstance = NULL;

static const QChar SOSLReservedQChars[] = {'\\', '?', '&', '|', '!', '{', '}', '[', ']', '(', ')', '^', '~', '*', ':', '"', '\'', '+', '-'};
static const QChar SOSLEscapeChar = '\\';

SFRestAPI::SFRestAPI() : QObject(0), mEndPoint(DefaultEndpoint), mApiVersion(""), mUserAgent(this->constructUserAgent()), mPendingTasks() {
	connect(SFAuthenticationManager::instance(), SIGNAL(SFOAuthFlowSuccess(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowSuccess(SFOAuthInfo*)));
	connect(SFAuthenticationManager::instance(), SIGNAL(SFOAuthFlowFailure(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowFailure(SFOAuthInfo*)));
	connect(SFAuthenticationManager::instance(), SIGNAL(SFOAuthFlowCanceled(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowCanceled(SFOAuthInfo*)));
}

SFRestAPI::~SFRestAPI() {
	for(QQueue<SFRestResourceTask*>::iterator i = mPendingTasks.begin(); i != mPendingTasks.end(); i++) {
		(*i)->deleteLater();
	}
}

/****************************
 * APIs
 ****************************/
SFRestAPI *SFRestAPI::instance() {
	if (!sharedInstance) {
		sharedInstance = new SFRestAPI();
	}
	return sharedInstance;
}

const SFOAuthCredentials* SFRestAPI::currentCredentials() {
	return SFAccountManager::instance()->getCoordinator()->getCredentials();
}

void SFRestAPI::sendRestRequest(SFRestRequest * request, QObject * resultReciever, const char * resultRecieverSlot, const QVariant & tag) {
	if (!request) {
		return;
	}

	SFRestResourceTask *task = this->createRestTask(request, tag);

	//we do manual connect here, in case we need to move the task to pending queue
	connect(task, SIGNAL(taskResultReady(sf::SFResult*)), resultReciever, resultRecieverSlot);

	this->startRestTask(task);

}


void SFRestAPI::sendRestRequest(SFRestRequest * request, const QScriptValue & resultReciever, const QScriptValue & resultRecieverSlot, const QVariant & tag) {
	if (!request || !resultReciever.isQObject() || !resultRecieverSlot.isFunction() || !resultRecieverSlot.engine()) {
		return;
	}

	SFRestResourceTask *task = this->createRestTask(request, tag);

	//to ensure the result is properly sent to QtScript, we register meta type.
	qScriptRegisterMetaType(resultRecieverSlot.engine(), SFResult::toScriptValue, SFResult::fromScriptValue);
	//we do manual connect here, in case we need to move the task to pending queue
	qScriptConnect(task, SIGNAL(taskResultReady(sf::SFResult*)), resultReciever, resultRecieverSlot);

	this->startRestTask(task);
}

SFRestRequest * SFRestAPI::requestForVersions() {
	//NOTE: if this function is called from QML/Javascript, the javascript engine will take ownership of the object
	return new SFRestRequest(0, "/", HTTPMethod::HTTPGet, "", this->mEndPoint, this->mUserAgent);
}

SFRestRequest * SFRestAPI::requestForResources() {
	return new SFRestRequest(0, "/", HTTPMethod::HTTPGet, this->mApiVersion, this->mEndPoint, this->mUserAgent);
}

SFRestRequest * SFRestAPI::requestForDescribeGlobal() {
	return new SFRestRequest(0, "/sobjects", HTTPMethod::HTTPGet, this->mApiVersion, this->mEndPoint, this->mUserAgent);
}

SFRestRequest * SFRestAPI::requestForMetadata(const QString & objectType) {
	return new SFRestRequest(0, QString("/sobjects/%1").arg(objectType), HTTPMethod::HTTPGet, this->mApiVersion, this->mEndPoint, this->mUserAgent);
}

SFRestRequest * SFRestAPI::requestForDescribeObject(const QString & objectType) {
	return new SFRestRequest(0, QString("/sobjects/%1/describe").arg(objectType), HTTPMethod::HTTPGet, this->mApiVersion, this->mEndPoint, this->mUserAgent);
}

SFRestRequest * SFRestAPI::requestForRetrieveObject(const QString & objectType, const QString & objectId, const QStringList & fieldList) {
	QString path = QString("/sobjects/%1/%2").arg(objectType, objectId);
	QString fieldsStr = fieldList.join(",");
	SFRestRequest * request = new SFRestRequest(0, path, HTTPMethod::HTTPGet, this->mApiVersion, this->mEndPoint, this->mUserAgent);
	if (fieldList.size() != 0) {
		QVariantMap params;
		params["fields"] = fieldsStr;
		request->setRequestParams(params);
		request->setParamsContentType(SFRestRequest::HTTPContentTypeUrlEncoded);
	}
	return request;
}

SFRestRequest * SFRestAPI::requestForCreateObject(const QString & objectType, const QVariantMap & fields) {
    QString path = QString("/sobjects/%1").arg(objectType);
    SFRestRequest * request = new SFRestRequest(0, path, HTTPMethod::HTTPPost, this->mApiVersion, this->mEndPoint, this->mUserAgent);
    request->setRequestParams(fields);
    request->setParamsContentType(SFRestRequest::HTTPContentTypeJSON);
	return request;
}

SFRestRequest * SFRestAPI::requestForUpdateObject(const QString & objectType, const QString & objectId, const QVariantMap & fields) {
    QString path = QString("/sobjects/%1/%2").arg(objectType, objectId);
    SFRestRequest * request = new SFRestRequest(0, path, HTTPMethod::HTTPPatch, this->mApiVersion, this->mEndPoint, this->mUserAgent);
    request->setRequestParams(fields);
    request->setParamsContentType(SFRestRequest::HTTPContentTypeJSON);
	return request;
}

SFRestRequest * SFRestAPI::requestForUpsertObject(const QString & objectType, const QString & extIdField, const QString & extIdValue, const QVariantMap & fields) {
    QString path = QString("/sobjects/%1/%2/%3").arg(objectType, extIdField, extIdValue);
    SFRestRequest * request = new SFRestRequest(0, path, HTTPMethod::HTTPPatch, this->mApiVersion, this->mEndPoint, this->mUserAgent);
    request->setRequestParams(fields);
    request->setParamsContentType(SFRestRequest::HTTPContentTypeJSON);
	return request;
}

SFRestRequest * SFRestAPI::requestForDeleteObject(const QString & objectType, const QString & objectId) {
    QString path = QString("/sobjects/%1/%2").arg(objectType, objectId);
    return new SFRestRequest(0, path, HTTPMethod::HTTPDelete, this->mApiVersion);
}

SFRestRequest * SFRestAPI::requestForQuery(const QString & soql) {
	SFRestRequest *request = new SFRestRequest(0, "/query", HTTPMethod::HTTPGet, this->mApiVersion, this->mEndPoint, this->mUserAgent);
	QVariantMap params;
	params["q"] = soql;
	request->setRequestParams(params);
	request->setParamsContentType(SFRestRequest::HTTPContentTypeUrlEncoded);
	return request;
}

SFRestRequest * SFRestAPI::requestForSearch(const QString & sosl) {
	SFRestRequest *request = new SFRestRequest(0, "/search", HTTPMethod::HTTPGet, this->mApiVersion, this->mEndPoint, this->mUserAgent);
	QVariantMap params;
	params["q"] = sosl;
	request->setRequestParams(params);
	request->setParamsContentType(SFRestRequest::HTTPContentTypeUrlEncoded);
	return request;
}

SFRestRequest * SFRestAPI::customRequest(const QString & path,
		const HTTPMethodType & method,
		const QVariantMap & params,
		const sf::SFRestRequest::HTTPContentType & contentType) {

	SFRestRequest *request = new SFRestRequest(0, path, method, "", "", this->mUserAgent);
	request->setRequestParams(params);
	request->setParamsContentType(contentType);
	return request;
}

SFRestRequest * SFRestAPI::customRequest(const QString & path, const QScriptValue & method, const QVariantMap & params, const QScriptValue & contentType) {
	//call C++ version
	return customRequest(path, HTTPMethodType(method.toInt32()), params, SFRestRequest::HTTPContentType(contentType.toInt32()));
}

/* Convenient APIs */
QString SFRestAPI::generateSOQLQuery(const QStringList & fields, const QString & sObjectType, const QString & where, const QStringList & groupBy, const QString & having, const QStringList & orderBy, const int & limit) {
	if (fields.isEmpty() || sObjectType.isNull() || sObjectType.isEmpty()) {
		return QString();
	}

	QString query = "";
	QTextStream tsQuery(&query);

	tsQuery << "SELECT " << fields.join(",") << " FROM " << sObjectType;

	if (!where.isNull() && !where.isEmpty()) {
		tsQuery << " WHERE " << where;
	}

	if (!groupBy.isEmpty()) {
		tsQuery << " GROUP BY " << groupBy.join(",");
		if (!having.isNull() && !having.isEmpty()) {
			tsQuery << " HAVING " << having;
		}
	}

	if (!orderBy.isEmpty()) {
		tsQuery << " ORDER BY " << orderBy.join(",");
	}

	if (limit > 0) {
		tsQuery << " LIMIT " << limit;
	}

	return query;
}

QString SFRestAPI::generateSOSLSearchQuery(const QString & searchTerm, const QString & fieldScope, const QVariantMap & objectScope, const int & limit) {
	if (searchTerm.isNull() || searchTerm.isEmpty()) {
		return QString();
	}
	QString query = "";
	QTextStream tsQuery(&query);

	//escape searchTerm
	QString escapedTerm = sanitizeSOSLSearchTerm(searchTerm);
	tsQuery << "FIND {"<< escapedTerm << "} " << (fieldScope.isNull() || fieldScope.isEmpty() ? "IN NAME FIELDS" : fieldScope);

	if (!objectScope.isEmpty()) {
		//key=sObject, value=scope
		QStringList scopes;
		for(QVariantMap::const_iterator i = objectScope.constBegin(); i != objectScope.constEnd(); i++) {
			if (i.value().canConvert(QVariant::String)) {
				scopes.append(QString("%1 (%2)").arg(i.key(), i.value().toString()));
			} else {
				scopes.append(i.key());
			}
		}
		tsQuery << " RETURNING " << scopes.join(", ");
	}

	if (limit > 0) {
		tsQuery << " LIMIT " << limit;
	}

	return query;
}

QString SFRestAPI::sanitizeSOSLSearchTerm(const QString & searchTerm) {
	size_t count = sizeof(SOSLReservedQChars) / sizeof(QChar);
	QString escapedTerm = searchTerm;
	for(size_t i=0; i<count; i++) {
		escapedTerm.replace(SOSLReservedQChars[i], QStringBuilder<QChar, QChar>(SOSLEscapeChar, SOSLReservedQChars[i]));
	}
	return escapedTerm;
}

/****************************
 * Protected Slots
 ****************************/
void SFRestAPI::onSFOAuthFlowSuccess(SFOAuthInfo*) {
	//send all pending task
	this->resendAllPendingTasks();
}

void SFRestAPI::onSFOAuthFlowFailure(SFOAuthInfo*) {
	//failed, remove all pending tasks
	//we run the task anyway. The task would eventually fail and trigger proper signals
	this->resendAllPendingTasks();
}

void SFRestAPI::onSFOAuthFlowCanceled(SFOAuthInfo*) {
	//cancelled, do same thing as failed
	this->resendAllPendingTasks();
}

void SFRestAPI::onTaskShouldRetry(SFGenericTask* genericTask, SFResult*) {
	if (genericTask->isAutoRetry()) {
		//why we care if it's auto retry?
		return;
	}

	SFRestResourceTask *task = qobject_cast<SFRestResourceTask*>(genericTask);
	if (!task) {
		//this shouldn't happen. don't retry, we delete it.
		sfWarning() << "[SFRestAPI] A non-SFRestResourceTask try to manually re-send";
		genericTask->deleteLater();
		return;
	}

	if (SFAuthenticationManager::instance()->isAuthenticating()) {
		//authentication in progress
		sfWarning() << "[SFRestAPI] Received 401. Authentication is in progress. Schedule to re-send later.";
		mPendingTasks.enqueue(task);
		return;
	}

	//check if current access token is same as the task used.
	//If it's not this implies the access token is updated while the task is running, we don't try login, re-run it immediately
	const SFOAuthCredentials* credential = this->currentCredentials();
	if (credential && !credential->getAccessToken().isNull() && !credential->getAccessToken().isEmpty()
			&& SFRestRequest::extractAccessToken(task->request()) != credential->getAccessToken()) {

		sfWarning() << "[SFRestAPI] Received 401. Access token has changed. Schedule to re-send immediately";
		QMetaObject::invokeMethod(task, "startTaskAsync", Qt::QueuedConnection);
		return;
	}

	//put it to pending task queue and try login
	sfWarning() << "[SFRestAPI] Received 401. Will refresh access token and schedule to re-send later";
	mPendingTasks.enqueue(task);
	SFAuthenticationManager::instance()->login();
}

/****************************
 * Protected
 ****************************/
SFRestResourceTask* SFRestAPI::createRestTask(SFRestRequest * request, const QVariant & tag) {
	SFRestResourceTask *task = new SFRestResourceTask(getSharedNetworkAccessManager(), request);
	if (!tag.isNull() && tag.isValid()) {
		task->putTag(kSFRestRequestTag, tag);
	}

	task->setRetryCount(1); // give it a change to refresh token and retry;
	connect(task, SIGNAL(taskShouldRetry(sf::SFGenericTask*, sf::SFResult*)), this, SLOT(onTaskShouldRetry(sf::SFGenericTask*, sf::SFResult*)));

	return task;
}

void SFRestAPI::startRestTask(SFRestResourceTask * task) {
	const SFOAuthCredentials* credential = this->currentCredentials();
	if (!credential || credential->getAccessToken().isNull() || credential->getAccessToken().isEmpty() || !credential->getInstanceUrl().isValid()) {
		//auto login
		mPendingTasks.enqueue(task);
		if (!SFAuthenticationManager::instance()->isAuthenticating()) {
			SFAuthenticationManager::instance()->login();
		}
	} else {
		task->startTaskAsync();
	}
}

void SFRestAPI::resendAllPendingTasks() {
	while(mPendingTasks.size() != 0) {
		SFRestResourceTask *task = mPendingTasks.dequeue();
		//we run the task anyway. The task would eventually fail and trigger proper signals
		QMetaObject::invokeMethod(task, "startTaskAsync", Qt::QueuedConnection);
	}
}

QString SFRestAPI::findWebKitUserAgent() {
	//TODO Do more research on this
	QNetworkRequest request;
	QByteArray header = QString("User-Agent").toUtf8();
	if (request.hasRawHeader(header)) {
		return QString(request.rawHeader(header));
	}

	return "";
}

using namespace bb;
using namespace bb::device;
using namespace bb::platform;
QString SFRestAPI::constructUserAgent() {

	HardwareInfo hardwareInfo;
	PlatformInfo platformInfo;

	QString userAgent = "SalesforceMobileSDK/%1 %2/%3 (%4) %5/%6 %7 %8";
	userAgent = userAgent.arg(SFMobileSDKVersion,
			"Blackberry",
			platformInfo.osVersion(),
			hardwareInfo.modelName(),
			Application::applicationName(),
			Application::applicationVersion(),
			SFMobileSDKNativeDesignator,
			this->findWebKitUserAgent());

	//for debug
	sfDebug() << "UserAgent:" << userAgent;

	return userAgent;
}

} /* namespace sf */
