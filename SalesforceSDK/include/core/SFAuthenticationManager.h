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

class SFAuthenticationManager : public QObject {
	Q_OBJECT

private:
	bb::cascades::Sheet* mAuthSheet;
	bb::cascades::ScrollView* mAuthWebViewContainer;
	bb::cascades::ActivityIndicator* mIndicator;
	bool mFirstAuthAfterLaunch;

public:
	static SFAuthenticationManager * instance();

	/*
	 * will emit either the success or failure signal
	 * the signals will only be emitted once, but the caller can have multiple slots associated to the signals
	 */
	Q_INVOKABLE void login();
	/*
	 *cancel authentication process (via oauth coordinator), clear account manager's state
	 */
	Q_INVOKABLE void logout();

	/*
	 * return boolean value indicate whether the authentication is in progress
	 */
	Q_INVOKABLE bool isAuthenticating();

	/*
	 * cancel in progress authentication (stops webview from loading, or aborts refresh token connection)
	 * the existing credential information is not changed.
	 */
	Q_INVOKABLE void cancelAuthentication();

	/*
	 * Usually you shouldn't need to get these values directly. They are provided here to help you debug
	 */
	QString loginHost() const;
	const SFIdentityData* getIdData() const;
	const SFOAuthCredentials* getCredentials() const;

public slots:
	void onAppStart();

signals:
	/*
	 * emitted by the login action. Not recommended to use with queued connection. If you do you are responsible to make sure
	 * the pointer is still valid when the signal is processed.
	 */
	void SFOAuthFlowSuccess(SFOAuthInfo* info);
	void SFOAuthFlowFailure(SFOAuthInfo* info);
	void SFOAuthFlowCanceled(SFOAuthInfo* info);
	/*
	 * emitted by the logout action
	 */
	void SFUserLoggedOut();
	/*
	 * emitted after login host change is processed
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
