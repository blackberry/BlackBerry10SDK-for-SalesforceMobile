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
 * SFPlugin.h
 *
 *  Created on: Dec 1, 2013
 *      Author: timshi
 */

#ifndef SFPLUGIN_H_
#define SFPLUGIN_H_

#include <bb/cascades/WebView>
#include <QObject>
#include "SFPluginResult.h"

namespace sf {
using namespace bb::cascades;

/*!
 * @class SFPlugin
 * @headerfile SFPlugin.h <hybrid/SFPlugin.h>
 * @brief The SFPlugin class is the base class for plugin implementations.
 *
 * @details
 * The class provides methods to communicate with javascript
 *
 * Usage
 * -----
 * Create a concrete subclass of this class
 * Declare the methods that can be accessed from javascript as Q_INVOKABLE. The method should take only one parameter of type SFInvokedUrlCommand
 * @c SFInvokedUrlCommand encapsulates the information important to the invocation when the call was made from javascript side
 * @code{.h}
 * class SFAuthPlugin : public SFPlugin{
 *	Q_OBJECT
 *  //your own member variable and method declaration as required
 *
 *  //declare the methods that can be invoked from javascript
 *  public:
 *	Q_INVOKABLE
 *	void login(const SFInvokedUrlCommand& command);
 * }
 * @endcode
 *
 * In the implementation of the method, call the native code.
 * In this case, because the login process is asynchronous,
 * we keep a reference to the callbackId so that after the login is completed we can retrieve the callbackId and execute the call back function in javascript
 * Note the plugin instance is set to receive the login success signal from @c SFAuthenticationManager
 * @code{.cpp}
 * void SFAuthPlugin::login(const SFInvokedUrlCommand& command){
 *	callbackMap.insert(command.getMethodName(), command.getCallbackId());
 *	SFAuthenticationManager::instance()->login();
 * }
 * @endcode
 *
 * When we received the login success signal we retrieve the callbackId and let javascript know that the call back should be executed.
 * @code{.cpp}
 * void SFAuthPlugin::onSFOAuthFlowSuccess(SFOAuthInfo* info){
 *	QString callbackId = callbackMap.value("login"); //matching method name
 *	QVariantMap message;
 *	message.insert(kPluginAuthType, info->getAuthType());
 *	message.insert(kPluginAuthStatus, SFAuth_SUCCESS);
 *	SFPluginResult result = SFPluginResult(SFCommandStatus_OK, message, false);
 *	if (!callbackId.isEmpty() && !callbackId.isNull()){
 *		this->sendPluginResult(result,callbackId);
 *		callbackMap.remove("login");
 *	}else{
 *		QString js = QString("%1(%2)").arg(kLoginDefaultSuccessCallback, variantToJSON(message));
 *		this->evalJavascript(js);
 *	}
 * }
 * @endcode
 *
 * \sa SFAuthPlugin, SFRestPlugin, SFInvokedUrlCommand, SFPluginResult, SFHybridApplicationui
 * \author Tim Shi
 */
class SFPlugin : public QObject{
	Q_OBJECT
public:
	SFPlugin(WebView* webView);
	virtual ~SFPlugin();

protected:
	WebView* mWebView;

protected:
	/*!
	 * @param js the javascript to be executed in the web view
	 */
	void evalJavascript(const QString& js);
	/*!
	 * Helper method for converitng a QVariant to json.
	 * @param map the QVariant to be serialized into json
	 * @return json string representing the map
	 */
	QString variantToJSON(const QVariant& map);
	/*!
	 * Send the result to web view
	 * @param result The result to be sent to web view, see @c SFPluginResult
	 * @param callbackId The id of the callback function to be executed in the web view's javascript
	 */
	void sendPluginResult(const SFPluginResult& result, const QString& callbackId);
};

} /* namespace sf */
#endif /* SFPLUGIN_H_ */
