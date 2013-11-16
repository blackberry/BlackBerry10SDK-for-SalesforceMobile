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
* SFAccountManager.h
*
*  Created on: Oct 15, 2013
*      Author: timshi
*/

#ifndef SFACCOUNTMANAGER_H_
#define SFACCOUNTMANAGER_H_

#include <qobject.h>
#include <QList>
#include <QString>
#include <bb/cascades/WebView>

namespace sf {
class SFOAuthCoordinator;
class SFOAuthCredentials;
class SFIdentityData;
class SFIdentityCoordinator;
class SFOAuthInfo;

using namespace bb::cascades;

/*!
 * @class SFAccountManager
 * @headerfile SFAccountManager.h <core/SFAccountManager>
 * @brief Singleton class that manages salesforce.com account info
 *
 * @details
 * @c SFAccountManager saves the oAuth credentials and uses it to work with the oAuth coordinator
 * An instance of SFAccountManager uses one active oAuth coordinator at any time and listens to the coordinator's signals
 * so that it can save or clear the account information appropriately.
 * It works with the IdCoordinator in similarly manner
 *
 * Usage
 * -----
 * @c SFAccountManager is primarily meant to be used by other SDK classes. Normally application code should not
 * need to use SFAccountManager directly.
 *
 * One potential usage of SFAccountManager is that if your application changes the salesforce consumer key or redirect url after
 * the app has been run. You will need to use SFAccountManager to programmatically detect the change and ask the user to logout
 * of the application to make the change take effect. However changing consumer key or redirect url after the app has been run is not recommended,
 * since this means your application is now connecting to a different back end and all the user data should be refreshed.
 */

class SFAccountManager : public QObject {
	Q_OBJECT

private:
	QString mAccountIdentifier;
	SFOAuthCoordinator* mCoordinator;
	SFIdentityCoordinator* mIdCoordinator;

protected:
	static SFAccountManager * sharedInstance;

public:
	/*!
	 * @return singleton instance of @c SFAccountManager
	 */
	static SFAccountManager* instance();
	/*!
	 * @return the value of the login host we are currently using, for Production and Sandbox this just reads from QSettings
	 * if the login host is "CUSTOM", will read from the custom login host field to get the actual login host.
	 */
	QString loginHost();
	/*!
	 * @param newClientId sets the clientId (consumer key). Should not be changed after app is started.
	 */
	static void setClientId(QString newClientId);
	/*!
	 * @param newRedirectUri sets the redirect url. Should not be changed after app is started.
	 */
	static void setRedirectUri(QString newRedirectUri);
	/*!
	 * @param newScopes sets the scopes to be associated with the access token.
	 */
	static void setScopes(QList<QString> newScopes);
	/*!
	 * @return the oauth coordinator used by this account
	 */
	SFOAuthCoordinator* getCoordinator();
	/*!
	 * @return the identity cooridnator used by this account
	 */
	SFIdentityCoordinator* getIdCoordinator();
	/*!
	 * @return the login host saved in settings
	 */
    Q_INVOKABLE
    QString getLoginHostFromAppSettings();
    /*!
     * @param value save the value to settings
     */
    Q_INVOKABLE
    void setLoginHostToAppSettings(const QString &value);
    /*!
     * @return custom host value from settings
     */
    Q_INVOKABLE
    QString getCustomHostFromAppSettings();
    /*!
     * @param set custom host value to settings
     */
    Q_INVOKABLE
    void setCustomHostToAppSettings(const QString &value);
    /*!
     * called by the settings QML page to update the login host when user chooses a value.
     */
    Q_INVOKABLE
    void updateLoginHost();

	/*!
	 * Clean up on application exit. Should only be used by SFAuthenticationManager
	 */
	void onAboutToQuit();
	/*!
	 * @param if true clears the account state of the given account (i.e. credentials, coordinator
	 * instances, etc.) If false new coordinator instances are created but existing credential info still
	 * lives (used when switching login hosts). This method should only be used by @c SFAuthenticationManager
	 */
	void clearAccountState(bool clearAccountData);
	/*!
	 * @return key for storing device passcode. Should only be used by SFSecurityLockout
	 */
    QString devicePasscodeKey();
signals:
	/*!
	 *pass on the signal from coordinator to SFAuthenticationManager
	 *application code should not be listening to these signals
	 *application code should listen to the Authentication Manager's signals instead.
	 */
	void willBeginAuthentication(SFOAuthInfo* info);
	void didBeginAuthentication(WebView* view);
	void oauthCoordinatorDidAuthenticate(SFOAuthInfo* info);
	void oauthCoordinatorDidFailWithError(QVariantMap error, SFOAuthInfo* info);
	void identityCoordinatorRetrievedData(SFIdentityCoordinator* coordinator);
	void identityCoordinatorDidFailWithError(QVariantMap error);
	void loginHostChanged();

protected:
	SFAccountManager();
	SFAccountManager(QString accountIdentifier);
	virtual ~SFAccountManager();

private:
	static SFAccountManager* instanceForAccount(QString accountIdentifier);
	static QString currentAccountIdentifier();
	static void setCurrentAccountIdentifier(QString newAccountIdentifier);
	static QString clientId();
	static QString redirectUri();
	static QList<QString> scopes();
	//This method will read the settings from the application's configuration
	void ensureAccountDefaultsExist();
	SFOAuthCredentials* getCredentials();
	SFIdentityData* getIdData();
	QString credentialKey();
	QString identityDataKey();
	void saveCredentials(SFOAuthCredentials* newCredentials);
	void saveIdentityData(SFIdentityData* newIdentityData);

private slots:
	/*
	 * handle oAuthCoordinator's signals
	 */
	void onWillBeginAuthentication(SFOAuthInfo* info);
	void onDidBeginAuthentication(WebView* view);
	void onOauthCoordinatorDidAuthenticate(SFOAuthCoordinator* coordinator, SFOAuthInfo* info);
	void onOauthCoordinatorDidFailWithError(QVariantMap error, SFOAuthInfo* info);
	void onIdentityCoordinatorRetrievedData(SFIdentityCoordinator* coordinator);
	void onIdentityCoordinatorDidFailWithError(QVariantMap error);
};

}/*name space*/
#endif /* SFACCOUNTMANAGER_H_ */
