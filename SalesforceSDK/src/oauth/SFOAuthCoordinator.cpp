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
* SFOAuthCoordinator.cpp
*
*  Created on: Oct 11, 2013
*      Author: timshi
*/

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <bb/cascades/WebNavigationRequest>
#include <bb/cascades/WebLoadRequest>
#include <bb/cascades/WebLoadStatus>
#include <bb/cascades/WebSettings>
#include <bb/cascades/WebStorage>
#include <bb/data/JsonDataAccess>
#include "SFOAuthCoordinator.h"
#include "SFOAuthInfo.h"
#include "SFOAuthCredentials.h"
#include "SFResult.h"
#include "SFGlobal.h"
#include "SFNetworkAccessTask.h"

namespace sf {

static const QString kSFOAuthEndPointAuthorize = "/services/oauth2/authorize";
static const QString kSFOAuthEndPointToken = "/services/oauth2/token";
static const QString kSFOAuthClientId = "client_id";
static const QString kSFOAuthRedirectUri = "redirect_uri";
static const QString kSFOAuthDisplay = "display";
static const QString kSFOAuthDisplayTouch = "touch";
static const QString kSFOAuthResponseType = "response_type";
static const QString kSFOAuthResponseTypeToken = "token";
static const QString kSFOAuthScope = "scope";
static const QString kSFOAuthRefreshToken = "refresh_token";
static const QString kSFOAuthId = "id";
static const QString kSFOAuthAccessToken  = "access_token";
static const QString kSFOAuthInstanceUrl = "instance_url";
static const QString kSFOAuthIssuedAt = "issued_at";
static const QString kSFOAuthFormat = "format";
static const QString kSFOAuthFormatJson = "json";
static const QString kSFOAuthGrantType = "grant_type";
static const QString kSFOAuthGrantTypeRefreshToken = "refresh_token";
static const QString kSFOAuthErrorTypeMalformedResponse = "malformed_response";
static const QString kSFOAuthErrorTypeJDA = "jda_data_access_error";
static const QString kSFOAuthErrorTypeWebRequest = "web_request_failed";
static const QString kSFOAuthErrorTypeInvalidCredentials = "invalid_credentials";

static const QString kHttpPostContentType = "application/x-www-form-urlencoded";

SFOAuthCoordinator::SFOAuthCoordinator() {
	mCredentials = NULL;
	mView = NULL;
	mIsAuthenticating = false;
	mInitialRequestLoaded = false;
	mAuthInfo = new SFOAuthInfo();
	mAuthInfo->setParent(this);
}

SFOAuthCoordinator::SFOAuthCoordinator(SFOAuthCredentials* credentials){
	mCredentials=credentials;
	credentials->setParent(this);
	mView = NULL;
	mIsAuthenticating = false;
	mInitialRequestLoaded = false;
	mAuthInfo = new SFOAuthInfo();
	mAuthInfo->setParent(this);
}

SFOAuthCoordinator::~SFOAuthCoordinator() {
	if (mView!=NULL){
		mView->deleteLater();
	}
}

/*
 *public apis
 */

//Based on whether we have a refresh token, start the refresh work flow or the user agent work flow
const SFOAuthInfo* SFOAuthCoordinator::authenticate(){
	if (mIsAuthenticating){
		sfWarning()<<"[SFOAuthCoordinator] authenticating process already started";
		return mAuthInfo;
	}

	mIsAuthenticating = true;

	if (mCredentials->getClientId().isEmpty() || mCredentials->getRedirectUrl().isEmpty()){
		sfWarning()<<"[SFOAuthCoordinator] The consumer key and redirect url cannot be empty";
		QVariantMap error;
		error.insert(kSFOAuthError, kSFOAuthErrorTypeInvalidCredentials);
		error.insert(kSFOAuthErrorDescription, "The consumer key and redirect url cannot be empty");
		notifyFailure(error, mAuthInfo);
		return mAuthInfo;
	}
	if (mCredentials->getRefreshToken()!=NULL){
		mAuthInfo->setAuthType(SFOAuthTypeRefresh);
		this->beginTokenRefreshFlow();
	}else{
		mAuthInfo->setAuthType(SFOAuthTypeUserAgent);
		this->beginUserAgentFlow();
	}
	return mAuthInfo;
}
bool SFOAuthCoordinator::isAuthenticating(){
	return mIsAuthenticating;
}
SFOAuthInfo* SFOAuthCoordinator::stopAuthentication(){
	sfDebug()<<"SFOAuthCoordinator::stopAuthentication";
	if (mView!=NULL){
		mView->stop();
	}
	emit cancelRefreshTask();

	mIsAuthenticating = false;
	return mAuthInfo;
}
void SFOAuthCoordinator::revokeAuthentication(){
	mCredentials->revoke();
}
QList<QString> SFOAuthCoordinator::getScopes(){
	return mScopes;
}
void SFOAuthCoordinator::setScopes(QList<QString> scopes){
	mScopes = scopes;
}

SFOAuthCredentials* SFOAuthCoordinator::getCredentials(){
	return mCredentials;
}
WebView* SFOAuthCoordinator::getView(){
	return mView;
}
SFOAuthInfo* SFOAuthCoordinator::getAuthInfo(){
	return mAuthInfo;
}
bool SFOAuthCoordinator::hasTokens(){
	return mCredentials->getRefreshToken()!=NULL;
}

/*
 * private methods
 */
void SFOAuthCoordinator::beginUserAgentFlow(){
	sfWarning()<<"[SFOAuthCoordinator] begin user agent flow";
	emit willBeginAuthentication(mAuthInfo);
	mInitialRequestLoaded = false;
	if (mView==NULL){
		mView = WebView::create();
	}
	mView->settings()->setCookiesEnabled(false);
	mView->storage()->clear();

	//this coordinator will handle the webview's call backs
	connect(mView, SIGNAL(navigationRequested(bb::cascades::WebNavigationRequest*)), this, SLOT(onNavigationRequested(bb::cascades::WebNavigationRequest*)), Qt::UniqueConnection);
	connect(mView, SIGNAL(loadingChanged(bb::cascades::WebLoadRequest*)), this, SLOT(onLoadingChanged(bb::cascades::WebLoadRequest*)),Qt::UniqueConnection);
	connect(mView, SIGNAL(destroyed (QObject *)), this, SLOT(onWebViewDestroyed(QObject *)), Qt::UniqueConnection);

	/*
	 * using the credentials to construct the url and load it in the webview
	 */
	QString approvalUrl = QString("%1://%2%3?%4=%5&%6=%7&%8=%9").arg(mCredentials->getProtocol(), mCredentials->getDomain(), kSFOAuthEndPointAuthorize, kSFOAuthClientId, mCredentials->getClientId(), kSFOAuthRedirectUri, mCredentials->getRedirectUrl(),kSFOAuthDisplay,kSFOAuthDisplayTouch);

	approvalUrl.append(QString("&%1=%2").arg(kSFOAuthResponseType,kSFOAuthResponseTypeToken));

	//add scopes to the url
	if (mScopes.count()>0){
		approvalUrl.append(QString("&%1=").arg(kSFOAuthScope));

		QString scopeStr = QString(kSFOAuthRefreshToken);

		QList<QString>::iterator i;
		for (i = mScopes.begin(); i != mScopes.end(); ++i){
			if (*i != kSFOAuthRefreshToken){
				scopeStr.append(QString(" %1").arg(*i));
			}
		}
		approvalUrl.append(QUrl::toPercentEncoding(scopeStr));
	}
	sfDebug()<<"approval url is " << approvalUrl;
	mView->setUrl(QUrl(approvalUrl));
}
void SFOAuthCoordinator::beginTokenRefreshFlow(){
	sfWarning()<<"[SFOAuthCoordinator] begin refresh token flow";

	//construct the request
	QString url = QString("%1://%2%3").arg(mCredentials->getProtocol(),mCredentials->getDomain(),kSFOAuthEndPointToken);
	QNetworkRequest request;
	request.setUrl(url);
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
	request.setHeader(QNetworkRequest::ContentTypeHeader, kHttpPostContentType);
	request.setAttribute(QNetworkRequest::CookieLoadControlAttribute, false);
	request.setAttribute(QNetworkRequest::CookieSaveControlAttribute, false);

	QString params = QString("%1=%2&%3=%4&%5=%6").arg(kSFOAuthFormat, kSFOAuthFormatJson,
            kSFOAuthRedirectUri, mCredentials->getRedirectUrl(),
            kSFOAuthClientId, mCredentials->getClientId());

	params.append(QString("&%1=%2&%3=%4").arg(kSFOAuthGrantType, kSFOAuthGrantTypeRefreshToken, kSFOAuthRefreshToken, mCredentials->getRefreshToken()));

	sfDebug()<<"refresh request params "<<params;

	SFNetworkAccessTask *task = new SFNetworkAccessTask(getSharedNetworkAccessManager(), request, HTTPMethod::HTTPPost);
	task->setRequestBytesArray(params.toUtf8());
	task->setCancellable(true);
	connect(this, SIGNAL(cancelRefreshTask()), task, SLOT(cancel()));
	task->startTaskAsync(this, SLOT(onRefreshReplyReady(sf::SFResult*)));
}

void SFOAuthCoordinator::handleRefreshResponse(QByteArray responseData){
	sfDebug()<<"json data is "<<responseData;
	bb::data::JsonDataAccess jda;
	QVariant jsonData = jda.loadFromBuffer(responseData);

	if (!jda.hasError()){
		QMap<QString, QVariant> dataMap = jsonData.toMap();
		mCredentials->setIdentityUrl(dataMap.value(kSFOAuthId).toString());
		mCredentials->setAccessToken(dataMap.value(kSFOAuthAccessToken).toString());
		mCredentials->setInstanceUrl(dataMap.value(kSFOAuthInstanceUrl).toString());

		notifySuccess(mAuthInfo);
	}else{
		sfDebug()<<"jda error"<<jda.error().errorMessage();
		QVariantMap error;
		error.insert(kSFOAuthError, kSFOAuthErrorTypeJDA+jda.error().errorType());
		error.insert(kSFOAuthErrorDescription, jda.error().errorMessage());
		notifyFailure(error, mAuthInfo);
	}
}

void SFOAuthCoordinator::notifySuccess(SFOAuthInfo* info){
	if (mIsAuthenticating){
		mIsAuthenticating = false;
		emit oauthCoordinatorDidAuthenticate(this, info);
	}
}

void SFOAuthCoordinator::notifyFailure(QVariantMap error, SFOAuthInfo* info){
	if (mIsAuthenticating){
		mIsAuthenticating = false;
		emit oauthCoordinatorDidFailWithError(error, info);
	}
}

/*
 * slots
 */
void SFOAuthCoordinator::onNavigationRequested(bb::cascades::WebNavigationRequest* request){
	sfDebug()<<"web view navigation requested with type: " << request->navigationType();
	if (!(request->navigationType() == WebNavigationType::Other
			|| request->navigationType() == WebNavigationType::OpenWindow)){
		mView->setTouchPropagationMode(TouchPropagationMode::PassThrough);
	}
	/*
	 * check if the request has the redirect
	 * 	check if there are response
	 * 		check if response is error
	 * 			parse the response or report error, emits signal and stop the page from loading
	 */
	QString requestUrlString = request->url().toString();
	if (requestUrlString.startsWith(mCredentials->getRedirectUrl())){
		sfWarning()<<"[SFOAuthCoordinator] result from SFDC is " << request->url().toString();
		QMap<QString, QString> params;
		if (request->url().fragment()!=NULL){
			QString response = request->url().fragment();
			params = parseQueryString(response);
		}else if(request->url().hasQuery()){
			QList<QPair<QString,QString> > paramList = request->url().queryItems();
			for (QList<QPair<QString,QString> >::iterator i = paramList.begin(); i!=paramList.end(); i++){
				params.insert(((QPair<QString,QString>)*i).first, ((QPair<QString,QString>)*i).second);
			}
		}else{
			QVariantMap error;
			error.insert(kSFOAuthError, kSFOAuthErrorTypeMalformedResponse);
			error.insert(kSFOAuthErrorDescription, "redirect response has no payload");
			notifyFailure(error, mAuthInfo);
		}

		if (params.value(kSFOAuthError)!=NULL){
			sfDebug()<<"error in sfdc response parameter"<<params.value(kSFOAuthError);
			QVariantMap error;
			error.insert(kSFOAuthError, params.value(kSFOAuthError));
			error.insert(kSFOAuthErrorDescription, params.value(kSFOAuthErrorDescription));
			notifyFailure(error, mAuthInfo);
		}else{
			mCredentials->setIdentityUrl(params.value(kSFOAuthId));
			mCredentials->setAccessToken(params.value(kSFOAuthAccessToken));
			mCredentials->setInstanceUrl(params.value(kSFOAuthInstanceUrl));
			mCredentials->setRefreshToken(params.value(kSFOAuthRefreshToken));
			mCredentials->setIssuedAt(params.value(kSFOAuthIssuedAt));

			this->notifySuccess(mAuthInfo);
		}

		//ignore the redirect because got the response so we are done
		request->ignore();
	}
}
void SFOAuthCoordinator::onLoadingChanged(bb::cascades::WebLoadRequest *loadRequest){
	sfDebug()<<"web view loading changed "<<loadRequest->url().toString();
	if (loadRequest->status() == WebLoadStatus::Started){
		sfDebug()<<"started for " << loadRequest->url().toString();
	}else if (loadRequest->status() == WebLoadStatus::Succeeded){
		mView->setTouchPropagationMode(TouchPropagationMode::Full);
		sfDebug()<<"succeeded for " << loadRequest->url().toString();
		if (!mInitialRequestLoaded){
			mView->settings()->setCookiesEnabled(true);
			mInitialRequestLoaded = true;
			emit didBeginAuthentication(mView);
		}
	}else if (loadRequest->status() == WebLoadStatus::Failed){
		mView->setTouchPropagationMode(TouchPropagationMode::Full);
		sfDebug()<<"failed for " << loadRequest->url().toString();
		QVariantMap error;
		error.insert(kSFOAuthError, kSFOAuthErrorTypeWebRequest);
		error.insert(kSFOAuthErrorDescription, "web view loading failed");
		//in case there are other request going on, stop them.
		mView->stop();
		notifyFailure(error, mAuthInfo);
	}
}

/*
 * TOKEN refresh flow
 */
void SFOAuthCoordinator::onRefreshReplyReady(sf::SFResult* result) {
	QByteArray replyData;
	if (result->hasError()) {
		sfDebug() << "refresh request finished with error:" << result->code() << result->message();
		QVariantMap error;
		error.insert(kSFOAuthError, result->code());
		error.insert(kSFOAuthErrorDescription, result->message());
		notifyFailure(error, mAuthInfo);
	} else {
		//normal flow
		sfDebug()<<"refresh request finished";
		QNetworkReply *reply = result->payload<QNetworkReply*>();
		if (reply) {
			replyData = reply->readAll();
		}
		handleRefreshResponse(replyData);
	}
}


void SFOAuthCoordinator::onWebViewDestroyed(QObject * obj){
	if (obj == mView){
		mView = NULL;
	}
}

/*
 * utilities
 */
QMap<QString, QString> SFOAuthCoordinator::parseQueryString(QString query){
	QList<QString> pairs = query.split("&");
	QList<QString>::iterator i;
	QMap<QString,QString> queryPairs;
	for (i=pairs.begin();i!=pairs.end();i++){
		QList<QString> keyValuePair = ((QString)*i).split("=");
		queryPairs.insert(keyValuePair.at(0),keyValuePair.at(1));
	}
	return queryPairs;
}

}
