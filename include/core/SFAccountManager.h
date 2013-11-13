/*
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

/*
 * Singleton class that manages salesforce account info
 * it saves and restores the oAuth credentials and uses that info to work with the oAuth coordinator
 * each one of this object has one oAuthCoordinator in its life time and it listens to some of oAuthCoordinator's signals
 * it works with the IdCoordinator in similarly manner
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
	static SFAccountManager* instance();
	// returns the value of the login host we are currently using, for Production and Sandbox this just reads from QSettings
    // if the login host is "CUSTOM", will read from the custom login host field to get the actual login host.
	QString loginHost();
	// sets the information critical for getting access
	// they should not be changed after app is started
	static void setClientId(QString newClientId);
	static void setRedirectUri(QString newRedirectUri);
	static void setScopes(QList<QString> newScopes);
	//do not delete the returned coordinator instance!
	SFOAuthCoordinator* getCoordinator();
	SFIdentityCoordinator* getIdCoordinator();
    /**
     * Following methods should only be used by the Setting pages
     */
    Q_INVOKABLE
    QString getLoginHostFromAppSettings();
    Q_INVOKABLE
    void setLoginHostToAppSettings(const QString &value);
    Q_INVOKABLE
    QString getCustomHostFromAppSettings();
    Q_INVOKABLE
    void setCustomHostToAppSettings(const QString &value);
    Q_INVOKABLE
    void updateLoginHost();

	// Should only be used by SFAuthenticationManager
	void onAboutToQuit();
	/* Should only be used by SFAuthenticationManager
	 *
	 * Clears the account state of the given account (i.e. clears credentials, coordinator
	 * instances, etc. Pass false when only the loginhost changed or retrying
	 */
	void clearAccountState(bool clearAccountData);
    // Should only be used by SFSecurityLockout
    QString devicePasscodeKey();
signals:
	//pass on the signal from coordinator to SFAuthenticationManager
	//application developer should not be listening to these signals
	//listen to the Authentication Manager's signals instead.
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
