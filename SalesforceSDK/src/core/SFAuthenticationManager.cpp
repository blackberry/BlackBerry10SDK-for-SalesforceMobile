/*
 * SFAuthenticationManager.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: timshi
 */

#include "SFAuthenticationManager.h"
#include "SFAccountManager.h"
#include <bb/cascades/Sheet>
#include <bb/cascades/Page>
#include <bb/cascades/ScrollView>
#include <bb/cascades/QmlDocument>
#include "SFGlobal.h"
#include "SFSecurityLockout.h"
#include "SFAccountManager.h"
#include "SFIdentityData.h"
#include "SFOAuthCredentials.h"
#include "SFOAuthInfo.h"
#include "SFOAuthCoordinator.h"
#include "SFIdentityCoordinator.h"
#include "SFIdentityData.h"
#include <bb/system/SystemToast>
#include <bb/cascades/WebLoadStatus>
#include <bb/cascades/WebLoadRequest>
#include <bb/cascades/WebNavigationRequest>

namespace sf {

SFAuthenticationManager * SFAuthenticationManager::sharedInstance = NULL;

/*
 * Public APIs
 */
SFAuthenticationManager* SFAuthenticationManager::instance() {
	if (!SFAuthenticationManager::sharedInstance) {
		SFAuthenticationManager::sharedInstance = new SFAuthenticationManager();
	    QmlDocument *qml = QmlDocument::create("asset:///salesforce/SFOAuthSheet.qml").parent(sharedInstance);
	    qml->setContextProperty("SFAuthenticationManager", sharedInstance);
	    Sheet* sheet = qml->createRootObject<Sheet>();
	    sharedInstance->setAuthSheet(sheet);
		sharedInstance->setScrollView(sheet->findChild<ScrollView*>("oauthScrollView"));
		sharedInstance->setIndicator(sheet->findChild<ActivityIndicator*>("indicator"));
		SFAccountManager* accountManagerInstance = SFAccountManager::instance();
		connect(accountManagerInstance, SIGNAL(loginHostChanged()), sharedInstance, SLOT(onLoginHostChanged()), Qt::UniqueConnection);
		SFSecurityLockout* securityLockoutInstance = SFSecurityLockout::instance();
		connect(securityLockoutInstance, SIGNAL(devicePasscodeCreated()), sharedInstance, SLOT(onDevicePasswordCreated()), Qt::UniqueConnection);
	}
	return sharedInstance;
}

void SFAuthenticationManager::setScopes(QList<QString> newScopes){
	SFAccountManager::setScopes(newScopes);
}

void SFAuthenticationManager::login(){
	SFAccountManager* accountManager = SFAccountManager::instance();
    connect(accountManager, SIGNAL(willBeginAuthentication(SFOAuthInfo*)), this, SLOT(onWillBeginAuthentication(SFOAuthInfo*)), Qt::UniqueConnection);
    connect(accountManager, SIGNAL(didBeginAuthentication(WebView*)), this, SLOT(onDidBeginAuthentication(WebView*)), Qt::UniqueConnection);
    connect(accountManager, SIGNAL(oauthCoordinatorDidAuthenticate(SFOAuthInfo*)), this, SLOT(onOauthCoordinatorDidAuthenticate(SFOAuthInfo*)), Qt::UniqueConnection);
    connect(accountManager, SIGNAL(oauthCoordinatorDidFailWithError(QVariantMap, SFOAuthInfo*)), this, SLOT(onOauthCoordinatorDidFailWithError(QVariantMap, SFOAuthInfo*)), Qt::UniqueConnection);
    accountManager->getCoordinator()->authenticate();
}

void SFAuthenticationManager::logout(){
	SFAccountManager::instance()->getCoordinator()->stopAuthentication();
	SFAccountManager::instance()->clearAccountState(true);
	SFSecurityLockout::instance()->reset();
	emit SFUserLoggedOut();
}

bool SFAuthenticationManager::isAuthenticating() {
	SFAccountManager* accountManager = SFAccountManager::instance();
	//the entire authentication process is not complete until the id retrieval is done.
	return (accountManager->getCoordinator()->isAuthenticating() || accountManager->getIdCoordinator()->isRetrievingData());
}

void SFAuthenticationManager::cancelAuthentication(){
	//also stop id retrieval if it's in progress
	SFAccountManager::instance()->getIdCoordinator()->cancelRetrieval();
	SFOAuthInfo* info = SFAccountManager::instance()->getCoordinator()->stopAuthentication();
	//in case we already got some data, clear them
	SFAccountManager::instance()->clearAccountState(true);
	SFSecurityLockout::instance()->reset();
	emit SFOAuthFlowCanceled(info);
}

void SFAuthenticationManager::onAppStart(){
	if (mFirstAuthAfterLaunch){
		sfDebug()<<"check login on app start";
		if (SFAccountManager::instance()->getCoordinator()->hasTokens()){
			if (!SFSecurityLockout::instance()->hasPasscode()){
				SFSecurityLockout::instance()->createPasscode();
			}else{
				SFSecurityLockout::instance()->lock();
			}
		}else{
			login();
		}
	}
	mFirstAuthAfterLaunch = false;
}

QString SFAuthenticationManager::loginHost() const{
	SFAccountManager* accountManager = SFAccountManager::instance();
	return accountManager->getLoginHostFromAppSettings();
}

const SFIdentityData* SFAuthenticationManager::getIdData() const{
	SFAccountManager* accountManager = SFAccountManager::instance();
	return accountManager->getIdCoordinator()->getIdData();
}

const SFOAuthCredentials* SFAuthenticationManager::getCredentials() const{
	SFAccountManager* accountManager = SFAccountManager::instance();
	return accountManager->getCoordinator()->getCredentials();
}

/*
 * private
 */
SFAuthenticationManager::SFAuthenticationManager() {
	mAuthSheet = NULL;
	mAuthWebViewContainer = NULL;
	mIndicator = NULL;
	mFirstAuthAfterLaunch = true;
}

SFAuthenticationManager::~SFAuthenticationManager() {
	if (mAuthSheet!=NULL){
		mAuthSheet->deleteLater();
		mAuthSheet = NULL;
	}
}

void SFAuthenticationManager::setAuthSheet(Sheet* sheet){
	mAuthSheet = sheet;
}
void SFAuthenticationManager::setScrollView(ScrollView* scrollView){
	mAuthWebViewContainer = scrollView;
}
void SFAuthenticationManager::setIndicator(bb::cascades::ActivityIndicator* indicator){
	mIndicator = indicator;
}
void SFAuthenticationManager::openAuthViewSheet(){
	mIndicator->setVisible(true);
	mIndicator->start();
	mAuthSheet->open();
}
void SFAuthenticationManager::presentAuthViewController(WebView* webView){
	if (mAuthWebViewContainer->content()!=NULL){
		//in case it wasn't removed already, remove ownership of the webview and remove the webview from view
		mAuthWebViewContainer->content()->setParent(NULL);
		mAuthWebViewContainer->setContent(NULL);
	}
	mAuthWebViewContainer->setContent(webView);
	webView->setHorizontalAlignment(HorizontalAlignment::Fill);
	webView->setVerticalAlignment(VerticalAlignment::Fill);
	connect(webView, SIGNAL(navigationRequested(bb::cascades::WebNavigationRequest*)), this, SLOT(onNavigationRequested(bb::cascades::WebNavigationRequest*)), Qt::UniqueConnection);
	connect(webView, SIGNAL(loadingChanged(bb::cascades::WebLoadRequest*)), this, SLOT(onLoadingChanged(bb::cascades::WebLoadRequest *)), Qt::UniqueConnection);
}
void SFAuthenticationManager::removeWebView(){
	//remove ownership of the webview and remove the webview from view
	//the coordinator takes care of the deletion of the webview
	if (mAuthWebViewContainer->content()!=NULL){
		mAuthWebViewContainer->content()->setParent(NULL);
		mAuthWebViewContainer->setContent(NULL);
	}
}
void SFAuthenticationManager::dismissAuthViewControllerIfPresent(){
	if (mAuthSheet!=NULL && mAuthSheet->isOpened()){
		mAuthSheet->close();
		removeWebView();
	}
}

/*
 * slots
 */
void SFAuthenticationManager::onWillBeginAuthentication(SFOAuthInfo* info){
	if (info->getAuthType()==SFOAuthTypeUserAgent){
		openAuthViewSheet();
	}
}

void SFAuthenticationManager::onDidBeginAuthentication(WebView* view){
	mIndicator->stop();
	mIndicator->setVisible(false);
	presentAuthViewController(view);
}
void SFAuthenticationManager::onOauthCoordinatorDidAuthenticate(SFOAuthInfo* info){
	if (info->getAuthType() == SFOAuthTypeUserAgent){
		SFAccountManager* accountManager = SFAccountManager::instance();
		connect(accountManager, SIGNAL(identityCoordinatorRetrievedData(SFIdentityCoordinator*)), this, SLOT(onIdentityCoordinatorRetrievedData(SFIdentityCoordinator*)), Qt::UniqueConnection);
		connect(accountManager, SIGNAL(identityCoordinatorDidFailWithError(QVariantMap)), this, SLOT(onIdentityCoordinatorDidFailWithError(QVariantMap)), Qt::UniqueConnection);
		accountManager->getIdCoordinator()->initiateIdentityDataRetrieval(
				accountManager->getCoordinator()->getCredentials());

		removeWebView();
		mIndicator->setVisible(true);
		mIndicator->start();
	}else if (info->getAuthType() != SFOAuthTypeUserAgent
			&& !SFSecurityLockout::instance()->isCreatingPasscode()){
		/*if user agent flow, we signal after the password is created.
		 *if it's refresh flow and the creating passcode screen is open
		 *that means the last time we didn't create passcode, so the signal will be emitted after that
		 */
		emit SFOAuthFlowSuccess(info);
	}
}
void SFAuthenticationManager::onOauthCoordinatorDidFailWithError(QVariantMap error, SFOAuthInfo* info){
	dismissAuthViewControllerIfPresent();

	bb::system::SystemToast *toast = new bb::system::SystemToast(this);
	toast->setBody(error.value(kSFOAuthErrorDescription).toString());
	toast->setPosition(bb::system::SystemUiPosition::BottomCenter);
	connect(toast, SIGNAL(finished(bb::system::SystemUiResult::Type)), toast, SLOT(deleteLater()));
	toast->show();

	emit SFOAuthFlowFailure(info);
	if (info->getAuthType() == SFOAuthTypeRefresh && error.value(kSFOAuthError).toInt() == 400){
		//if refresh flow got bad request, it means the refresh token is no longer valid
		//we will handle this by logging out so that the app can start over again.
		sfWarning()<<"[SFAuthenticationManager] logging out because refresh flow failed - refresh token likely was revoked on server";
		bb::system::SystemToast *toast = new bb::system::SystemToast(this);
		toast->setBody("Logging out because you have been logged in from too many devices");
		toast->setPosition(bb::system::SystemUiPosition::BottomCenter);
		connect(toast, SIGNAL(finished(bb::system::SystemUiResult::Type)), toast, SLOT(deleteLater()));
		toast->show();
		logout();
	}else if (info->getAuthType() == SFOAuthTypeUserAgent){
		mIndicator->stop();
		mIndicator->setVisible(false);
	}
}
void SFAuthenticationManager::onAboutToQuit(){
	mAuthSheet->deleteLater();
	SFAccountManager::instance()->onAboutToQuit();
	SFSecurityLockout::instance()->onAboutToQuit();
	if (SFAccountManager::instance()->getIdCoordinator()->isRetrievingData()){
		//quit while retrieving id data, we have to assume this login is in complete
		//we have to start over again next time.
		SFAccountManager::instance()->clearAccountState(true);
	}
}
void SFAuthenticationManager::onLoginHostChanged(){
	SFAccountManager::instance()->getCoordinator()->stopAuthentication();
	SFAccountManager::instance()->clearAccountState(false);
	SFSecurityLockout::instance()->cancelLock(); //the app is responsible for calling login and restart the lock process again.
	mFirstAuthAfterLaunch = true;
	emit SFLoginHostChanged();
}
void SFAuthenticationManager::onIdentityCoordinatorRetrievedData(SFIdentityCoordinator* coordinator){
	sfDebug()<<"pin length: " << coordinator->getIdData()->getMobileAppPinLength();
	sfDebug()<<"time out length: "<< coordinator->getIdData()->getMobileAppScreenLockTimeout();
	//this is only called on the user agent flow (which means the user has logged out
	if (!SFSecurityLockout::instance()->hasPasscode()
			&& coordinator->getIdData()->getMobileAppPinLength().toInt() > 0
			&& coordinator->getIdData()->getMobileAppScreenLockTimeout().toInt() > 0){
		SFSecurityLockout* lockout = SFSecurityLockout::instance();
		lockout->createPasscode();
		connect(lockout->getCreatePasscodeSheet(), SIGNAL(opened()), this, SLOT(dismissAuthViewControllerIfPresent()), Qt::UniqueConnection);
	}else{
		dismissAuthViewControllerIfPresent();
		emit SFOAuthFlowSuccess(SFAccountManager::instance()->getCoordinator()->getAuthInfo());
	}
}
void SFAuthenticationManager::onIdentityCoordinatorDidFailWithError(QVariantMap error){
	dismissAuthViewControllerIfPresent();

	bb::system::SystemToast *toast = new bb::system::SystemToast(this);
	toast->setBody(error.value(kSFOAuthErrorDescription).toString());
	toast->setPosition(bb::system::SystemUiPosition::BottomCenter);
	connect(toast, SIGNAL(finished(bb::system::SystemUiResult::Type)), toast, SLOT(deleteLater()));
	toast->show();

	//since we didn't succeed, we have to start over again next time.
	SFAccountManager::instance()->clearAccountState(true);
	SFSecurityLockout::instance()->reset();

	emit SFOAuthFlowFailure(SFAccountManager::instance()->getCoordinator()->getAuthInfo());
}
void SFAuthenticationManager::onDevicePasswordCreated(){
	emit SFOAuthFlowSuccess(SFAccountManager::instance()->getCoordinator()->getAuthInfo());
}
void SFAuthenticationManager::onNavigationRequested(bb::cascades::WebNavigationRequest* request){
	if (!(request->navigationType() == WebNavigationType::Other
			|| request->navigationType() == WebNavigationType::OpenWindow)){
		mIndicator->setVisible(true);
		mIndicator->start();
	}
}
void SFAuthenticationManager::onLoadingChanged(bb::cascades::WebLoadRequest *loadRequest){
	if (loadRequest->status() == WebLoadStatus::Failed || loadRequest->status() == WebLoadStatus::Succeeded){
		mIndicator->setVisible(false);
		mIndicator->stop();
	}else{
		mIndicator->setVisible(true);
		mIndicator->start();
	}
}
}/*namespace*/
