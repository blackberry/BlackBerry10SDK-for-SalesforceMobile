/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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

using namespace bb::cascades;

/*!
 * @class SFAuthPlugin
 * @headerfile SFAuthPlugin <hybrid/SFAuthPlugin.h>
 * @brief The SFAuthPlugin class makes the methods on @c SFAuthenticationManager available to javascript
 *
 * @details
 * The javascript running in the webview that is registered with this plugin can invoke the methods on @c SFAuthenticationManager via this plugin.
 * Each function can be invoked with success and failure callbacks. In addition if log in or log out occured inside the app without being implicitly invoked
 * from the javascript side, the plugin will notify the javascript default call back functions if they are defined.
 *
 * The default callbacks for login state changes are:
 *
 * <br>onSFOAuthFlowSuccess(message)
 * <br>onSFOAuthFlowFailure(message)
 * <br>onSFOAuthFlowCancel(message)
 * <br>onSFUserLoggedOut
 * <br>onSFLoginHostChanged
 *
 * Usage
 * -----
 * Calling the login method from javascript
 * @code {.html}
 * <html>
 *	<head>
 *		<script type="text/javascript" src="sf.js"></script>
 *		<script type="text/javascript">
 *			//default handler if no callback is provided
 *			function onSFOAuthFlowSuccess(message){
 *				document.getElementById("output").innerHTML = "<p>logged in success:" + JSON.stringify(message) + "</p>";
 *			}
 *			//plugin was executed successfully
 *			function loginSuccess(message) {
 *				if (message.authStatus == 0){ //see SFAuthPlugin.h for the status enum
 *				    sf.exec(credentialSuccess,credentialFailure,"sf::SFAuthPlugin", "getCredentials", {});
 *				}else if (message.authStatus == 2){ //see SFAuthPlugin.h for the status enum
 *				    document.getElementById("output").innerHTML = "<p>login canceled</p>";
 *				}else {
 *				    document.getElementById("output").innerHTML = "<p>login failed</p>";
 *				}
 *			}
 *			//if plugin wasn't able to be executed successfully
 *			function loginFail(message) {
 *			    document.getElementById("output").innerHTML = "<p>login failed: " + message + "</p>";
 *			}
 *			//trigger the plugin's method
 *			function login(){
 *				document.getElementById("output").innerHTML = "<p>authenticating</p>";
 *				sf.exec(loginSuccess,loginFail,"sf::SFAuthPlugin", "login", {});
 *			}
 * 		</script>
 *	</head>
 *	<body>
 *		<div class="interactive" id="login" ontouchend='login()' style="background-color:#cc3f10">login to salesforce</div>
 *		<div id="output"></div>
 *	</body>
 * </html>
 * @endcode
 *
 * \sa SFRestPlugin, SFInvokedUrlCommand, SFPluginResult, SFHybridApplicationui
 * \author Tim Shi
 */

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
	void onSFLoginHostChanged();

public:
	//Q_INVOKABLE so that the MOC will include it
	/*!
	 * Logs in the user, invokable from javascript.
	 * The javascript callback will get a message of the following form in the callback
	 * {
	 * 	"authType":0,
	 * 	"authStatus":0
	 * }
	 * See SFOAuthType, and SFAuthStatus enum for possible values.
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE void login(const SFInvokedUrlCommand& command);
	/*!
	 * logs out the user, invokable from javascript
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE void logout(const SFInvokedUrlCommand& command);
	/*!
	 * Get the @c SFOAuthCredentials, invokable from javascript
	 * The message passed back to javascript is the json representation of @c SFOAuthCredentials
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE void getCredentials(const SFInvokedUrlCommand& command);
};
} /* namespace sf */
#endif /* SFAUTHPLUGIN_H_ */
