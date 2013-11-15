/*
 * SFOAuthCoordinator.h
 *
 *  Created on: Oct 11, 2013
 *      Author: timshi
 */

#ifndef SFOAUTHCOORDINATOR_H_
#define SFOAUTHCOORDINATOR_H_

#include <QObject>
#include <QList>
#include <QMap>
#include <QString>
#include <bb/cascades/WebView>
#include <QtNetwork/QNetworkReply>

namespace sf {

class SFOAuthCredentials;
class SFOAuthInfo;
class SFResult;

using namespace bb::cascades;

/*!
 * @class SFOAuthCoordinator
 * @headerfile SFOAuthCoordinator.h <oauth/SFOAuthCoordinator.h>
 *
 * @brief The `SFOAuthCoordinator` class is the central class of the OAuth2 authentication process.

 * @details
 * This class manages a web view instance and monitors it as it works its way
 * through the various stages of the OAuth2 workflow. When authentication is complete,
 * the coordinator instance extracts the necessary session information from the response
 * and updates the `SFOAuthCredentials` object as necessary.
 *
 * This class is used by the @c SFAccountManager class.
 */
class SFOAuthCoordinator : public QObject {
	Q_OBJECT
/*
 * APIs
 */
public:
	SFOAuthCoordinator();
	SFOAuthCoordinator(SFOAuthCredentials* credentials);
	virtual ~SFOAuthCoordinator();

	/*!
	 * @return the type of authentication process that was started (user-agent or refresh token flow)
	 * Begins the authentication process.
	 */
	const SFOAuthInfo* authenticate();
	/*!
	 * @return true if the coordinator is in the process of authentication; otherwise NO.
	 */
	bool isAuthenticating();
	/*!
	 * @return the type of authentication process that was started (user-agent or refresh token flow)
	 * Stops the authentication process.
	 */
	SFOAuthInfo* stopAuthentication();
	/*!
	 * Revokes the authentication credentials by removing them from device.
	 */
	void revokeAuthentication();
	/*!
	 * @return the set of scopes for oauth
	 */
	QList<QString> getScopes();
	/*!
	 * @param scopes
	 * See:
	 * https://help.salesforce.com/apex/HTViewHelpDoc?language=en&id=remoteaccess_oauth_scopes.htm
 	 * Generally you need not specify this unless you are using something other than the "api" scope.
 	 * For instance, if you are accessing Visualforce pages as well as the REST API, you could use:
  	 * [@"api", @"visualforce"]
 	 * (You need not specify the "refresh_token" scope as this is always requested by this library.)
 	 * If you do not set this property, the library does not add the "scope" parameter to the initial
 	 * OAuth request, which implicitly sets the scope to include: "id", "api", and "refresh_token".
 	 */
	void setScopes(QList<QString> scopes);
	/*!
	 * @return User credentials to use within the authentication process.
	 */
	SFOAuthCredentials* getCredentials();
	/*!
	 * @return View in which the user will input OAuth credentials for the user-agent flow OAuth process.
	 */
	WebView* getView();
	/*!
	 * Returns the current @c SFAuthInfo
	 */
	SFOAuthInfo* getAuthInfo();
	/*!
	 * @return true if the coordinator has tokens already.
	 */
	bool hasTokens();
	/*!
	 * @param query
	 * @return query parameters parsed into a map.
	 */
	static QMap<QString, QString> parseQueryString(QString query);

signals:
	/*returning pointers, the caller that connects to the signal should make
	 *sure that the objects are still alive when the signal is delivered.
	 *this is normally the SFAccountManager/SFAuthenticationManager
	 *note: an coordinator instance is destroyed when the SFAccountManager instance that holds it clears its account state
	 *and its signal and slots are automatically disconnected
	 */
	void willBeginAuthentication(SFOAuthInfo* info);
	void didBeginAuthentication(WebView* view);
	//SFOAuthInfo is used to pass information, we don't expect the slot to modify its value, so it's passed by value
	void oauthCoordinatorDidAuthenticate(SFOAuthCoordinator* coordinator, SFOAuthInfo* info);
	void oauthCoordinatorDidFailWithError(QVariantMap error, SFOAuthInfo* info);
	void cancelRefreshTask();
private:
	SFOAuthCredentials* mCredentials;
	QList<QString> mScopes;
	WebView* mView;
	bool mIsAuthenticating;
	bool mInitialRequestLoaded;
	SFOAuthInfo* mAuthInfo;

	/*
	 * private functions
	 */
	void beginUserAgentFlow();
	void beginTokenRefreshFlow();
	void handleRefreshResponse(QByteArray responseData);
	void notifySuccess(SFOAuthInfo* info);
	void notifyFailure(QVariantMap error, SFOAuthInfo* info);

private slots:
	void onNavigationRequested(bb::cascades::WebNavigationRequest* webview);
	void onLoadingChanged(bb::cascades::WebLoadRequest *loadRequest);
	void onWebViewDestroyed(QObject * obj);
	void onRefreshReplyReady(sf::SFResult*);
};

}
#endif /* SFOAUTHCOORDINATOR_H_ */
