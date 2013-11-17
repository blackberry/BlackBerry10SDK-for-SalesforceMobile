/*
* Copyright 2013 BlackBerry Limited.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* SFRestRequest.cpp
*
*  Created on: Oct 17, 2013
*      Author: Livan Yi Du
*/

#include "SFRestRequest.h"
#include "SFGlobal.h"
#include "SFAccountManager.h"
#include "SFOAuthCoordinator.h"
#include "SFOAuthCredentials.h"
#include "SFRestAPI.h"
#include <bb/data/JsonDataAccess>

namespace sf {

const QString DefaultEndpoint = "/services/data";
static const QString ContentTypeUrlEncoded = "application/x-www-form-urlencoded; charset=utf-8";
static const QString ContentTypeJSON = "application/json; charset=utf-8";
static const QString ContentTypeMultiPart = "multipart/form-data";

static const QString kAccessTokenHeader = "Authorization";
static const QString AccessTokenPrefix = "Bearer ";

SFRestRequest::SFRestRequest(QObject * parent, const QString & path, const HTTPMethodType & method, const QString & apiVersion, const QString & endPoint, const QString & userAgent)
: QObject(parent), mRequestParams(), mParamsContentType(HTTPContentTypeUrlEncoded), mRequestRawData(), mRequestRawHeaders() {
	this->mMethod = method;
	this->mEndPoint = endPoint;
	this->mApiVersion = apiVersion;
	this->mPath = path;
	if (userAgent.isNull() || userAgent.isEmpty()) {
		this->mUserAgent = SFRestAPI::instance()->userAgent();
	} else {
		this->mUserAgent = userAgent;
	}
}

SFRestRequest::~SFRestRequest() {

}

QVariant SFRestRequest::param(const QString & key) {
	if (key.isNull() || !this->mRequestParams.contains(key)) {
		return QVariant();
	}
	return this->mRequestParams[key];
}

void SFRestRequest::setParam(const QString & key, const QVariant & value) {
	if (key.isNull()) {
		return;
	}

	if (value.isNull() || !value.isValid()) {
		this->removeParam(key);
	} else {
		this->mRequestParams[key] = value;
	}

}

void SFRestRequest::removeParam(const QString & key) {
	if (key.isNull() || !this->mRequestParams.contains(key)) {
		return;
	}
	this->mRequestParams.remove(key);
}

SFResultCodeType SFRestRequest::prepareNetworkRequest(QNetworkRequest *pOutRequest, QByteArray *pOutData, QString *pOutErrorMsg) {
	if (!pOutRequest || !pOutData) {
		if (pOutErrorMsg) *pOutErrorMsg = "[SFRestRequest] Arguments pOutRequest & pOutData cannot be NULL.";
		return SFResultCode::SFErrorGeneric;
	}

	//create request
	SFResultCodeType errorCode = this->createNetworkRequest(pOutRequest, pOutErrorMsg);
	if (errorCode != SFResultCode::SFErrorNoError) {
		return errorCode;
	}

	//additional settings
	if (!mUserAgent.isNull() && !mUserAgent.isEmpty()) {
		pOutRequest->setRawHeader(QString("User-Agent").toUtf8(), mUserAgent.toUtf8());
	}

	//add additional headers if available
	for(QVariantMap::const_iterator i = mRequestRawHeaders.constBegin(); i != mRequestRawHeaders.constEnd(); i++) {
		if (!i.key().isNull() && i.value().type() != QVariant::ByteArray && i.value().canConvert<QString>()) {
			pOutRequest->setRawHeader(i.key().toUtf8(), i.value().value<QString>().toUtf8());
		} else if (!i.key().isNull() && i.value().canConvert<QByteArray>()) {
			pOutRequest->setRawHeader(i.key().toUtf8(), i.value().value<QByteArray>());
		}
	}

	//set request data
	if (!this->mRequestRawData.isNull() && !this->mRequestRawData.isEmpty()) {
		*pOutData = this->mRequestRawData;
		//NOTE: if raw data is created, we assume "Content-Type" is set in headers

	} else if (!this->mRequestParams.isEmpty()) {
		if (mMethod <= HTTPMethod::HTTPDelete) {

			//if method is get, head or delete, we always encode params to URL
			this->mParamsContentType = HTTPContentTypeUrlEncoded;
			QUrl url(pOutRequest->url());
			bool ret = this->encodeParamsToURL(url);
			if (!ret) {
				if (pOutErrorMsg) *pOutErrorMsg = "[SFRestRequest] Invalid request parameters.";
				return SFResultCode::SFErrorNetwork;
			}
			pOutRequest->setUrl(url);

		} else {

			//for other method we put params to body data
			QString contentType;
			bool ret = this->encodeParamsToData(pOutData, &contentType);
			if (!ret) {
				if (pOutErrorMsg) *pOutErrorMsg = "[SFRestRequest] Invalid request parameters.";
				return SFResultCode::SFErrorNetwork;
			}
			pOutRequest->setHeader(QNetworkRequest::ContentTypeHeader, contentType.toUtf8());
		}
	}

	//for debug
	sfDebug() << "[SFRestRequest] Request Summary:\n"<< this->composeRequestSummary(*pOutRequest, *pOutData);
	return SFResultCode::SFErrorNoError;
}

QString SFRestRequest::extractAccessToken(const QNetworkRequest & request) {
	if (!request.hasRawHeader(kAccessTokenHeader.toUtf8())) {
		return QString();
	}

	QString headerValue = request.rawHeader(kAccessTokenHeader.toUtf8());
	if (!headerValue.startsWith(AccessTokenPrefix)) {
		return QString();
	}

	return headerValue.mid(AccessTokenPrefix.length());
}

/***********************
 * Privates
 ***********************/
SFResultCodeType SFRestRequest::createNetworkRequest(QNetworkRequest *pOutRequest, QString * pOutErrorMsg) {

	SFResultCodeType errorCode = SFResultCode::SFErrorNoError;
	QString errorMsg;
	const SFOAuthCredentials* credentials = SFRestAPI::instance()->currentCredentials();
	QString accessToken = credentials->getAccessToken();
	QUrl base = credentials->getInstanceUrl();
	QString urlTemplate = QString("%1%2%3");
	QUrl relativeUrl(urlTemplate.arg(mEndPoint.isNull() ? "":mEndPoint, mApiVersion.isNull() ? "":mApiVersion, mPath.isNull() ? "":mPath));

	//validate some important info: instance Url, access token and overall path
	if (!relativeUrl.isValid() || relativeUrl.isEmpty()) {
		errorCode = SFResultCode::SFErrorNetwork;
		errorMsg = QString("Invalid relative path: '%1'").arg(relativeUrl.toString());
	} else if (accessToken.isNull() || accessToken.isEmpty()) {
		errorCode = SFResultCode::SFErrorInvalidAccessToken;
		errorMsg = QString("Access token is not available. Did you forget to authenticate first?");
	} else if ((base.isEmpty() || !base.isValid()) && relativeUrl.isRelative()) {
		errorCode = SFResultCode::SFErrorInvalidAccessToken;
		errorMsg = QString("Instance URL is not valid: %1").arg(base.toString());
	}

	if (errorCode != 0) {
		if (pOutErrorMsg) *pOutErrorMsg = errorMsg;
		return errorCode;
	}

	// now construct the request
	QUrl url = relativeUrl.isRelative() ? base.resolved(relativeUrl) : relativeUrl;
	QNetworkRequest request(url);

	//insert access token into header
	QString headerValue = QString("%1%2").arg(AccessTokenPrefix, accessToken);
	request.setRawHeader(kAccessTokenHeader.toUtf8(), headerValue.toUtf8());

	if (pOutRequest) *pOutRequest = request;
	return errorCode;
}

bool SFRestRequest::encodeParamsToURL(QUrl & url) {
	if (!url.isValid()) {
		return false;
	}

		for(QVariantMap::const_iterator i = mRequestParams.constBegin(); i != mRequestParams.constEnd(); i++) {
			if (i.key().isNull() || i.key().isEmpty()) {
				sfWarning() << "[SFRestRequest] Invalid parameter key";
				continue;
			}

			QString value;
			if (i.value().type() != QVariant::ByteArray && i.value().canConvert<QString>()) {
				value = i.value().value<QString>();
			} else if (i.value().canConvert<QByteArray>()) {
				value = i.value().value<QByteArray>().toBase64();
			} else {
				sfWarning() << "[SFRestRequest] Invalid parameter value:" << i.value();
				continue;
			}

			url.addEncodedQueryItem(i.key().toUtf8(), QUrl::toPercentEncoding(value));
		}

	return true;
}

bool SFRestRequest::encodeParamsToData(QByteArray * pOutBytes, QString * pOutContentType) {
	if (!pOutBytes || !pOutContentType) {
		return false;
	}

	switch(this->mParamsContentType) {
	case HTTPContentTypeJSON: {
		*pOutBytes = QByteArray(); //clean byte array
		using namespace bb::data;
		JsonDataAccess jda;
		jda.saveToBuffer(QVariant::fromValue<QVariantMap>(mRequestParams), pOutBytes);
		if (jda.hasError()) {
			return false;
		}
		*pOutContentType = ContentTypeJSON;
		return true;
	}
	case HTTPContentTypeMultiPart: {
		//Currently not supported
		*pOutContentType = ContentTypeMultiPart;
		return false;
	}
	case HTTPContentTypeUrlEncoded:
	default: {
		this->createUrlEncodedContent(pOutBytes);
		*pOutContentType = ContentTypeUrlEncoded;
		return true;
	}
	}
}

void SFRestRequest::createUrlEncodedContent(QByteArray *pOutBytes) {

	QString content = "";
	for(QVariantMap::const_iterator i = mRequestParams.constBegin(); i != mRequestParams.constEnd(); i++) {
		if (i.key().isNull() || i.key().isEmpty()) {
			sfWarning() << "[SFRestRequest] Invalid parameter key";
			continue;
		}

		QString value;
		if (i.value().type() != QVariant::ByteArray && i.value().canConvert<QString>()) {
			value = i.value().value<QString>();
		} else if (!i.value().isNull() && i.value().canConvert<QByteArray>()) {
			value = i.value().value<QByteArray>().toBase64();
		} else {
			sfWarning() << "[SFRestRequest] Invalid parameter value:" << i.value();
			continue;
		}

		//No need to encode, it will go into request body
		if (i == mRequestParams.constBegin()) {
			content += QString("%1=%2").arg(i.key(), value);
		} else {
			content += QString("&%1=%2").arg(i.key(), value);
		}
	}

	*pOutBytes = content.toUtf8();
}

QString SFRestRequest::composeRequestSummary(const QNetworkRequest & request, QByteArray & data) {
	QString content = "URL: %1\nHeaders:\n%2\nContent:\n%3";
	content = content.arg(request.url().toString());

	QString headers;
	QList<QByteArray> rawHeaders = request.rawHeaderList();
	for(QList<QByteArray>::const_iterator i = rawHeaders.constBegin(); i != rawHeaders.constEnd(); i++) {
		QString entry = QString("%1 = %2\n").arg(QString(*i), QString(request.rawHeader(*i)));
		headers += entry;
	}
	content = content.arg(headers);

	content = content.arg(QString(data));
	return content;
}

} /* namespace sf */
