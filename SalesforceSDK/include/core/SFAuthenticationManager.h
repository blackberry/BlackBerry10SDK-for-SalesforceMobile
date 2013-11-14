/*
 * SFAuthenticationManager.h
 *
 *  Created on: Oct 15, 2013
 *      Author: timshi
 */

#ifndef SFAUTHENTICATIONMANAGER_H_
#define SFAUTHENTICATIONMANAGER_H_

#include <QObject>
#include <bb/cascades/WebView>
#include <bb/cascades/Sheet>
#include <bb/cascades/ScrollView>
#include <bb/cascades/Label>
#include <bb/cascades/ActivityIndicator>
/*
 * A singleton class whose main purpose is to give user API such as login and logout, and handle the UI change
 */
namespace sf {

class SFAccountManager;
class SFIdentityData;
class SFIdentityCoordinator;
class SFOAuthCredentials;
class SFOAuthInfo;
class SFOAuthCoordinator;

using namespace bb::cascades;

/*!
 * @class SFAuthenticationManager
 * @headerfile SFAuthenticationManager.h "SFAuthenticationManager.h"
 * @brief A singleton class that manages the oAuth 2.0 user agent/refresh flow
 *
 * @details
 * This is a singleton class and its instance can be accessed by by @c SFAuthenticationManager::instance()
 *
 * Initialization
 * ---------------------
 * Before using this class to start the authentication process, the application code needs to specify the required
 * information to connect to salesforce.com:
 * - consumer key: set this value in your @c applicationui class by defining the SFRemoteAccessConsumerKey variable
 * - redirect url: set this value in your @c applicationui class by defining the SFRemoteAccessConsumerKey variable
 * - scopes: optionally set the scopes you want the access token to be associated with. Set its value by calling @c SFAuthenticationManager::setScopes(QList<QString> newScopes);
 *
 * Note
 * ---------------------
 * The consumer key, redirect url and scopes information is persisted on the device.
 * If you decide to reconfigured these information, the change will take effect only
 * after the app performed a logout or the app is deleted and re-installed.
 *
 * Usage
 * -----
 * To use this class, connect to the desired signals and invoke the desired functions
 * @code{.cpp}
 * SFAuthenticationManager* authManager = SFAuthenticationManager::instance();
 * connect(authManager, SIGNAL(SFOAuthFlowSuccess(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowSuccess(SFOAuthInfo*)), Qt::UniqueConnection);
 * authManager->login();
 * @endcode
 *
 * In order for the SDK to handle the authentication process. It uses a few QML assets to construct the UI needed during authentication
 * They can be found in the assets/salesforce folder
 * - SFSettings.qml: a screen for user to specify which environment to connect the application to. The application developer is responsible for presenting this screen if a setting screen is required in the application.
 * 	 The SDK will do the necessary clean up tasks and emit a LoginHostChanged signal.
 * 	 The application is responsible for handling this signal (e.g. trigger login again)
 * - SFOAuthSheet.qml: a sheet that holds the web view for authentication. It is presented and dismissed by SFAuthenticationManager as part of the login process
 * - SFPasscodeCreation.qml: a sheet that lets user set up device password after authentication. It is presented and dismissed by SFAuthenticationManager
 * - SFLockScreen.qml: a sheet that locks the screen when the app is inactive for a period of time. It is presented and dismissed by SFAuthenticationManager
 *
 * \sa SFAuthenticationManager, SFAbstractApplicationUI
 * See [Authenticating Remote Access Application OAuth] (https://help.salesforce.com/apex/HTViewHelpDoc?id=remoteaccess_authenticate.htm&language=en_US) for more details.
 * This SDK supports the oAuth 2.0 user agent flow and refresh token flow.
 *
 * \author Tim Tian Le Shi
 */
class SFAuthenticationManager : public QObject {
	Q_OBJECT

private:
	bb::cascades::Sheet* mAuthSheet;
	bb::cascades::ScrollView* mAuthWebViewContainer;
	bb::cascades::ActivityIndicator* mIndicator;
	bool mFirstAuthAfterLaunch;

public:
	/*! @returns the singleton instance */
	static SFAuthenticationManager * instance();
	/*!
	 * @param a list of oAuth scope strings
	 */
	static void setScopes(QList<QString> newScopes);
	/*!
	 * Starts the authentication process and emits either the success or failure signal
	 * This function will trigger the user-agent flow if the refresh token is not available
	 * otherwise the refresh token flow will be triggered.
	 */
	Q_INVOKABLE void login();
	/*!
	 *Cancels any authentication in progress and clears stored account data
	 */
	Q_INVOKABLE void logout();

	/*!
	 * @return boolean value indicating whether there is any authentication in progress
	 */
	Q_INVOKABLE bool isAuthenticating();

	/*!
	 * Cancels in progress authentication (stops web view from loading, or aborts refresh token connection)
	 * Does not clear existing account data.
	 */
	Q_INVOKABLE void cancelAuthentication();

	/*! @return the current login host.*/
	QString loginHost() const;
	/*! @return the current logged in user's identity data*/
	const SFIdentityData* getIdData() const;
	/*! @return the current logged in user's credentials*/
	const SFOAuthCredentials* getCredentials() const;

public slots:
	/*!
	 * This slot is connected to the application's fullscreen signal
	 * to automatically display either the login screen or lock screen.
	 */
	void onAppStart();

signals:
	/*!
	 * @param a pointer to @c SFOAuthInfo indicating the type of the oAuth flow (user agent or refresh token)
	 * Emitted by @c login() if succeeded. Not recommended to use with queued connection. If you do you are responsible to make sure
	 * the pointer is still valid when the signal is processed.
	 */
	void SFOAuthFlowSuccess(SFOAuthInfo* info);
	/*!
	 * @param a pointer to @c SFOAuthInfo indicating the type of the oAuth flow (user agent or refresh token)
	 * Emitted by @c login() if failed
	 */
	void SFOAuthFlowFailure(SFOAuthInfo* info);
	/*!
	 * @param a pointer to @c SFOAuthInfo indicating the type of the oAuth flow (user agent or refresh token)
	 * Emitted by @c cancelAuthentication()
	 */
	void SFOAuthFlowCanceled(SFOAuthInfo* info);
	/*!
	 * Emitted by @c logout()
	 */
	void SFUserLoggedOut();
	/*!
	 * Emitted if user went into settings and changed the login host
	 */
	void SFLoginHostChanged();
protected:
	static SFAuthenticationManager * sharedInstance;

private:
	SFAuthenticationManager();
	virtual ~SFAuthenticationManager();

	void setAuthSheet(bb::cascades::Sheet* sheet);
	void setScrollView(bb::cascades::ScrollView* scrollView);
	void setIndicator(bb::cascades::ActivityIndicator* indicator);
	/*
	 * use sheet to present a modal view
	 */
	void openAuthViewSheet();
	void presentAuthViewController(WebView* webView);
	void removeWebView();

private slots:
	void dismissAuthViewControllerIfPresent();
	void onWillBeginAuthentication(SFOAuthInfo* info);
	void onDidBeginAuthentication(WebView* view);
	void onOauthCoordinatorDidAuthenticate(SFOAuthInfo* info);
	void onOauthCoordinatorDidFailWithError(QVariantMap error, SFOAuthInfo* info);
	void onAboutToQuit();
	void onLoginHostChanged();
	void onIdentityCoordinatorRetrievedData(SFIdentityCoordinator* coordinator);
	void onIdentityCoordinatorDidFailWithError(QVariantMap error);
	void onDevicePasswordCreated();
	//For showing the indicator while loading web content
	void onNavigationRequested(bb::cascades::WebNavigationRequest* request);
	void onLoadingChanged(bb::cascades::WebLoadRequest *loadRequest);
};

}/* namespace sf*/

#endif /* SFAUTHENTICATIONMANAGER_H_ */
