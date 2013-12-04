/*
 * SFAuthPlugin.h
 *
 *  Created on: Nov 27, 2013
 *      Author: timshi
 */

#ifndef SFAUTHPLUGIN_H_
#define SFAUTHPLUGIN_H_

#include <bb/cascades/WebView>
#include <QMap>
#include "SFOAuthInfo.h"
#include "SFPlugin.h"
#include "SFInvokedUrlCommand.h"

namespace sf {

typedef enum {
    SFAuth_SUCCESS = 0,
    SFAuth_FAIL,
    SFAuth_CANCEL
} SFAuthStatus;

/*An plugin should have the following characteristics
 *
 * 1. belongs to one webview (and has a reference to it)
 * 2. has methods that can be called from javascript
 * 3. has internal methods to write back to the javascript
 * 4. has internal methods to write success back to javascript, and invoke a callback
 * 5. has internal methods to write failure back to javascript, and invoke a callback
 *
 * if the plugin is asynchronous, keep track of the callerbackId
 *
 * all the methods that can be invoked from js should take SFInvokedUrlCommand as input
 */
using namespace bb::cascades;
class SFAuthPlugin : public SFPlugin{
	Q_OBJECT
public:
	SFAuthPlugin(WebView* webView);
	virtual ~SFAuthPlugin();

private:
	QMap<QString, QString> callbackMap;

protected slots:
	void onSFOAuthFlowSuccess(SFOAuthInfo* info);
	void onSFOAuthFlowFailure(SFOAuthInfo* info);
	void onSFOAuthFlowCanceled(SFOAuthInfo* info);
	void onSFUserLoggedOut();

public:
	//Q_INVOKABLE so that the MOC will include it
	Q_INVOKABLE
	void login(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void logout(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void getCredentials(const SFInvokedUrlCommand& command);
};
} /* namespace sf */
#endif /* SFAUTHPLUGIN_H_ */
