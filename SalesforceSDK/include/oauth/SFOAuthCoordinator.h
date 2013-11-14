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

class SFOAuthCoordinator : public QObject {
	Q_OBJECT
/*
 * APIs
 */
public:
	SFOAuthCoordinator();
	SFOAuthCoordinator(SFOAuthCredentials* credentials);
	virtual ~SFOAuthCoordinator();

	const SFOAuthInfo* authenticate();
	bool isAuthenticating();
	SFOAuthInfo* stopAuthentication();
	void revokeAuthentication();

	QList<QString> getScopes();
	void setScopes(QList<QString> scopes);
	SFOAuthCredentials* getCredentials();
	WebView* getView();
	SFOAuthInfo* getAuthInfo();
	bool hasTokens();
	/*
	 * utilities
	 */
	static QMap<QString, QString> parseQueryString(QString query);

/*
 * Signals - this would be equivalent to the delegate functions in the iOS implementation
 */
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
