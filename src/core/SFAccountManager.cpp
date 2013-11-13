/*
 * SFAccountManager.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: timshi
 */

#include "SFAccountManager.h"
#include <QSettings>
#include "SFOAuthCoordinator.h"
#include "SFOAuthCredentials.h"
#include "SFIdentityData.h"
#include "SFIdentityCoordinator.h"
#include "SFGlobal.h"

namespace sf {

static const QString SFDefaultAccountIdentifier = "Default";
// The key for storing the persisted OAuth client ID.
static const QString kOAuthClientIdKey = "oauth_client_id";
// The key for storing the persisted OAuth redirect URI.
static const QString kOAuthRedirectUriKey = "oauth_redirect_uri";
// The key for storing the persisted OAuth scopes.
static const QString kOAuthScopesKey = "oauth_scopes";
static const QString kOAuthCredentialsDataKeyPrefix = "oauth_credentials_data";
static const QString kOAuthIdentityDataKeyPrefix = "oauth_identity_data";
static const QString kDevicePasscodeKeyPrefix = "device_passcode";
// login host key to get the user selected login host in application setting page
static const QString kAppSettingsLoginHostKey = "app_setting_login_host";
// to get the user input custom login host
static const QString kAppSettingsCustomHostKey = "app_setting_custom_host";
// default login host if nothing is selected
static const QString kDefaultLoginHost = "login.salesforce.com";
// key to the login host we are using (to identify if the user has selected a different value)
static const QString kLoginHostKey = "login_host_pref";
// Value for kAppSettingsLoginHostKey when a custom host is chosen. this matches with the value in Settings.qml
static const QString kAppSettingsLoginHostIsCustom = "CUSTOM";


SFAccountManager * SFAccountManager::sharedInstance = NULL;
QString CurrentAccountIdentifier;

SFAccountManager::SFAccountManager() {
	mCoordinator = NULL;
	mIdCoordinator = NULL;
}

SFAccountManager::SFAccountManager(QString accountIdentifier) {
	ensureAccountDefaultsExist();
	mCoordinator = NULL;
	mIdCoordinator = NULL;
	mAccountIdentifier = accountIdentifier;
}

SFAccountManager::~SFAccountManager() {
}

SFAccountManager* SFAccountManager::instance() {
	if (!SFAccountManager::sharedInstance) {
		SFAccountManager::setCurrentAccountIdentifier(SFDefaultAccountIdentifier);
		SFAccountManager::sharedInstance = instanceForAccount(CurrentAccountIdentifier);
	}
	return sharedInstance;
}

SFAccountManager* SFAccountManager::instanceForAccount(QString accountIdentifier) {
	if (!SFAccountManager::sharedInstance) {
		SFAccountManager::sharedInstance = new SFAccountManager(accountIdentifier);
	}
	return sharedInstance;
}

void SFAccountManager::ensureAccountDefaultsExist(){
	QSettings setting;
	setting.setValue(kLoginHostKey, loginHost());
}

QString SFAccountManager::loginHost(){
	QString loginHost = getLoginHostFromAppSettings();
	if (loginHost==kAppSettingsLoginHostIsCustom){
		loginHost = getCustomHostFromAppSettings();
	}
	return loginHost;
}

/*
 * Credential management methods
 */

QString SFAccountManager::currentAccountIdentifier(){
	return CurrentAccountIdentifier;
}
void SFAccountManager::setCurrentAccountIdentifier(QString newAccountIdentifier){
	CurrentAccountIdentifier = newAccountIdentifier;
}

QString SFAccountManager::clientId(){
	QSettings setting;
	return setting.value(kOAuthClientIdKey).toString();
}
void SFAccountManager::setClientId(QString newClientId){
	QSettings setting;
	setting.setValue(kOAuthClientIdKey,newClientId);
}
QString SFAccountManager::redirectUri(){
	QSettings setting;
	return setting.value(kOAuthRedirectUriKey).toString();
}
void SFAccountManager::setRedirectUri(QString newRedirectUri){
	QSettings setting;
	setting.setValue(kOAuthRedirectUriKey,newRedirectUri);
}
QList<QString> SFAccountManager::scopes(){
	QSettings setting;
	QList<QVariant> scopesInSetting = setting.value(kOAuthScopesKey).toList();
	QList<QString> scopes;

	for (QList<QVariant>::iterator i = scopesInSetting.begin(); i!=scopesInSetting.end();i++){
		scopes.append((*i).toString());
	}
	return scopes;
}
void SFAccountManager::setScopes(QList<QString> newScopes){
	QSettings setting;
	QList<QVariant> scopes;
	for (QList<QString>::iterator i = newScopes.begin(); i!=newScopes.end();i++){
		scopes.append(QVariant((*i)));
	}
	setting.setValue(kOAuthScopesKey, scopes);
}

SFOAuthCoordinator* SFAccountManager::getCoordinator() {
	//mCoordinator is constructed here and should be deleted when it's set to null (in the clearState method)
	if (mCoordinator == NULL){
		sfDebug()<<"instantiating new oauth coordinator";
		mCoordinator = new SFOAuthCoordinator(getCredentials());
		mCoordinator->setScopes(scopes());
		mCoordinator->setParent(this);
	}

    connect(mCoordinator, SIGNAL(willBeginAuthentication(SFOAuthInfo*)), this, SLOT(onWillBeginAuthentication(SFOAuthInfo*)), Qt::UniqueConnection);
    connect(mCoordinator, SIGNAL(didBeginAuthentication(WebView*)), this, SLOT(onDidBeginAuthentication(WebView*)), Qt::UniqueConnection);
    connect(mCoordinator, SIGNAL(oauthCoordinatorDidAuthenticate(SFOAuthCoordinator*, SFOAuthInfo*)), this, SLOT(onOauthCoordinatorDidAuthenticate(SFOAuthCoordinator*, SFOAuthInfo*)), Qt::UniqueConnection);
    connect(mCoordinator, SIGNAL(oauthCoordinatorDidFailWithError(QVariantMap, SFOAuthInfo*)), this, SLOT(onOauthCoordinatorDidFailWithError(QVariantMap, SFOAuthInfo*)), Qt::UniqueConnection);

	return mCoordinator;
}

SFIdentityCoordinator* SFAccountManager::getIdCoordinator(){
	if (mIdCoordinator == NULL){
		mIdCoordinator = new SFIdentityCoordinator(getIdData());
		mIdCoordinator->setParent(this);
	}
	connect(mIdCoordinator, SIGNAL(identityCoordinatorRetrievedData(SFIdentityCoordinator*)), this, SLOT(onIdentityCoordinatorRetrievedData(SFIdentityCoordinator*)), Qt::UniqueConnection);
	connect(mIdCoordinator, SIGNAL(identityCoordinatorDidFailWithError(QVariantMap)), this, SLOT(onIdentityCoordinatorDidFailWithError(QVariantMap)), Qt::UniqueConnection);
	return mIdCoordinator;
}

//credential's ownership belongs to coordinator (set when it's passed to coordinator, so its memory is managed by the coordinator
SFOAuthCredentials* SFAccountManager::getCredentials(){
	SFOAuthCredentials* credentials = new SFOAuthCredentials();
	sfDebug()<<"try to unarchive credentials from device with key " << credentialKey();
	if (!credentials->unArchiveValueFromFile(credentialKey())){
		sfDebug()<<"unable to get old credentials, creating new one with default values";
		credentials->setClientId(clientId());
		credentials->setRedirectUrl(redirectUri());
		credentials->setDomain(loginHost());
		credentials->setIdentifier(credentialKey());
		saveCredentials(credentials);
	}
	return credentials;
}

SFIdentityData* SFAccountManager::getIdData(){
	SFIdentityData* idData = new SFIdentityData();
	sfDebug()<<"try to unarchive identity data from device with key " << identityDataKey();
	idData->unArchiveValueFromFile(identityDataKey());
	return idData;
}

void SFAccountManager::saveCredentials(SFOAuthCredentials* credentials){
	if (credentials == NULL){
		sfDebug()<<"remove credentials from device with key " << credentialKey();
		SFOAuthCredentials::removeCredentialArchive(credentialKey());
	}else{
		sfDebug()<<"save credentials to device with key "<< credentialKey();
		credentials->archiveValueToFile(credentialKey());
	}
}

void SFAccountManager::saveIdentityData(SFIdentityData* newIdentityData){
	if (newIdentityData == NULL){
		SFIdentityData::removeIdentityArchive(identityDataKey());
	}else{
		newIdentityData->archiveValueToFile(identityDataKey());
	}
}

QString SFAccountManager::credentialKey(){
	//the credential is keyed using prefix+loginhost+account identifier
	return QString("%1-%2-%3").arg(kOAuthCredentialsDataKeyPrefix, loginHost(), currentAccountIdentifier());
}

QString SFAccountManager::identityDataKey(){
	//the credential is keyed using prefix+loginhost+account identifier
	return QString("%1-%2-%3").arg(kOAuthIdentityDataKeyPrefix, loginHost(), currentAccountIdentifier());
}

QString SFAccountManager::devicePasscodeKey(){
	return QString("%1-%2-%3").arg(kDevicePasscodeKeyPrefix, loginHost(), currentAccountIdentifier());
}

void SFAccountManager::clearAccountState(bool clearAccountData){
	if (clearAccountData){
		if (mCoordinator!=NULL){
			mCoordinator->revokeAuthentication();
		}
		saveCredentials(NULL); //reset the persisted credentials
		if (mIdCoordinator!=NULL){
			mIdCoordinator->cancelRetrieval();
		}
		saveIdentityData(NULL);
	}
	//note: the caller is responsible of removing the oauth view if it's being presented
	//TODO: for hybrid app call authentication manager to remove cookies
	mCoordinator->deleteLater();
	mCoordinator = NULL;
	mIdCoordinator->deleteLater();
	mIdCoordinator = NULL;
}
void SFAccountManager::onAboutToQuit(){
	if (mCoordinator->getView()!=NULL){
		//otherwise we get a "webview not unrealized" message on app exit
		mCoordinator->getView()->deleteLater();
	}
}

/*
 * Account Settings
 */
QString SFAccountManager::getLoginHostFromAppSettings(){
	QSettings settings;
	QString loginHost = settings.value(kAppSettingsLoginHostKey).toString();
	if (loginHost.isNull() || loginHost.isEmpty()){
		return kDefaultLoginHost;
	}
	return loginHost;
}

/**
 * This function sets a value in the QSettings database.
 */
void SFAccountManager::setLoginHostToAppSettings(const QString &value){
	QSettings settings;
	settings.setValue(kAppSettingsLoginHostKey, value);
}

QString SFAccountManager::getCustomHostFromAppSettings(){
	QSettings settings;
	return settings.value(kAppSettingsCustomHostKey).toString();
}

void SFAccountManager::setCustomHostToAppSettings(const QString &value){
	QSettings settings;
	settings.setValue(kAppSettingsCustomHostKey, value);
}

void SFAccountManager::updateLoginHost(){
	QSettings setting;
	QString previousLoginHost = setting.value(kLoginHostKey).toString();
	QString currentLoginHost = loginHost();
	setting.setValue(kLoginHostKey, currentLoginHost);
	if (previousLoginHost!=currentLoginHost){
		sfWarning()<<"[SFAccountManager] update login host from "<<previousLoginHost<<" to "<<currentLoginHost;
		//this signal should be handled by authentication manager, which should cancel authentication, and clear account state (keep data)
		emit loginHostChanged();
	}
}

/*
 * Slots
 */
void SFAccountManager::onWillBeginAuthentication(SFOAuthInfo* info){
	emit willBeginAuthentication(info);
}
void SFAccountManager::onDidBeginAuthentication(WebView* view){
	emit didBeginAuthentication(view);
}
void SFAccountManager::onOauthCoordinatorDidAuthenticate(SFOAuthCoordinator* coordinator, SFOAuthInfo* info){
	saveCredentials(coordinator->getCredentials());
	emit oauthCoordinatorDidAuthenticate(info);
}
void SFAccountManager::onOauthCoordinatorDidFailWithError(QVariantMap error, SFOAuthInfo* info){
	emit oauthCoordinatorDidFailWithError(error, info);
}
void SFAccountManager::onIdentityCoordinatorRetrievedData(SFIdentityCoordinator* coordinator){
	saveIdentityData(coordinator->getIdData());
	emit identityCoordinatorRetrievedData(coordinator);
}
void SFAccountManager::onIdentityCoordinatorDidFailWithError(QVariantMap error){
	emit identityCoordinatorDidFailWithError(error);
}
}/*name space*/
