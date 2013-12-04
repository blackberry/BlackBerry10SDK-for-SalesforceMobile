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
