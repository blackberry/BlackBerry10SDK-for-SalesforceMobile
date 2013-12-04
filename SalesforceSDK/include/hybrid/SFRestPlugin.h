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
 * SFRestPlugin.h
 *
 *  Created on: Dec 2, 2013
 *      Author: timshi
 */

#ifndef SFRESTPLUGIN_H_
#define SFRESTPLUGIN_H_

#include "SFPlugin.h"
#include "SFRestAPI.h"
#include <QList>
#include "SFInvokedUrlCommand.h"

namespace sf {
/*!
 * @class SFRestPlugin
 * @headerfile SFRestPlugin <hybrid/SFRestPlugin.h>
 * @brief The SFRestPlugin class makes the methods on @c SFRestAPI available to javascript
 *
 * @details
 * The javascript running in the webview that is registered with this plugin can invoke the methods on @c SFRestAPI via this plugin. Each function can be invoked with success and failure callbacks.
 * Refer to @c SFRestAPI to see what arguments are expected to be passed in when calling these methods from javascript
 *
 * Usage
 * -----
 * Calling the login method from javascript
 * @code {.html}
 * <html>
 *	<head>
 *		<script type="text/javascript" src="sf.js"></script>
 *		<script type="text/javascript">
 *			function restRequestSuccess(message){
 *				document.getElementById("output").innerHTML = "<p>" + JSON.stringify(message) + "</p>";
 *			}
 *			function restRequestFail(message){
 *				document.getElementById("output").innerHTML = "<p>" + message + "</p>";
 *			}
 *			function sendRestRequest(){
 *				document.getElementById("output").innerHTML = "sending rest request";
 *				var args = {"soql":"SELECT Name FROM Account LIMIT 1"};
 *				sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForQuery",args);
 *			}
 * 		</script>
 *	</head>
 *	<body>
 *		<div class="interactive" id="rest" ontouchend='sendRestRequest()' style="background-color:#cc3f10">Send Rest Request</div>
 *		<div id="output"></div>
 *	</body>
 * </html>
 * @endcode
 *
 * \sa SFAuthPlugin, SFInvokedUrlCommand, SFPluginResult, SFHybridApplicationui
 * \author Tim Shi
 */
class SFRestPlugin : public SFPlugin{
	Q_OBJECT
public:
	SFRestPlugin(WebView* webView);
	virtual ~SFRestPlugin();
	/*!
	 * Used to invoke custom rest request
	 * @code{.js}
	 * 	var args = {"path":"/services/data/v28.0/query",
	 * 		"method":HTTPMethodType.HTTPGet,
	 * 		"params":{"q":"SELECT Name FROM Account LIMIT 1"},
	 * 		"contentType":HTTPContentType.HTTPContentTypeUrlEncoded}; //you can also pass params and contentType
	 * 	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendCustomRequest",args);
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendCustomRequest(const SFInvokedUrlCommand& command); //custom rest request
	/*!
	 * @code{.js}
	 * 	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForVersions",{});
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendRequestForVersions(const SFInvokedUrlCommand& command);
	/*!
	 * @code{.js}
	 * 	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForResources",{});
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendRequestForResources(const SFInvokedUrlCommand& command);
	/*!
	 * @code{.js}
	 * 	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForDescribeGlobal",{});
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendRequestForDescribeGlobal(const SFInvokedUrlCommand& command);
	/*!
	 * @code{.js}
	 * 	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForMetadata",{"objectType":"account"});
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendRequestForMetadata(const SFInvokedUrlCommand& command);
	/*!
	 * @code{.js}
	 * 	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForDescribeObject",{"objectType":"account"});
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendRequestForDescribeObject(const SFInvokedUrlCommand& command);
	/*!
	 * @code{.js}
	 *	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForRetrieveObject",{"objectType":"account", "objectId":"001E000000lcB2mIAE", "fields":["accountsource"]});
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendRequestForRetrieveObject(const SFInvokedUrlCommand& command);
	/*!
	 * @code{.js}
	 * 	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForCreateObject",{"objectType":"account", "fields":{"name":"html5 account"}});
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendRequestForCreateObject(const SFInvokedUrlCommand& command);
	/*!
	 * @code{.js}
	 * 	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForUpdateObject",{"objectType":"account", "objectId":"001E000000lcB2mIAE", "fields":{"name":"html5 account new"}});
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendRequestForUpdateObject(const SFInvokedUrlCommand& command);
	/*!
	 * @code{.js}
	 * 	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForUpsertObject",{"objectType":"account", "extIdField":"id", "extIdValue":"001E000000lcB2mIAE", "fields":{"name":"html5 account new upsert"}});
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendRequestForUpsertObject(const SFInvokedUrlCommand& command);
	/*!
	 * @code{.js}
	 * 	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForDeleteObject",{"objectType":"account", "objectId":"001E000000lcB2mIAE"});
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendRequestForDeleteObject(const SFInvokedUrlCommand& command);
	/*!
	 * @code{.js}
	 * 	document.getElementById("output").innerHTML = "sending rest request";
	 * 	var args = {"soql":"SELECT Name FROM Account LIMIT 1"};
	 * 	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForQuery",args);
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendRequestForQuery(const SFInvokedUrlCommand& command);
	/*!
	 * @code{.js}
	 *	sf.exec(restRequestSuccess,restRequestFail,"sf::SFRestPlugin","sendRequestForSearch",{"sosl":"Find {test}"});
	 * @endcode
	 * @param command The command passed in from javascript
	 */
	Q_INVOKABLE
	void sendRequestForSearch(const SFInvokedUrlCommand& command);

private slots:
	void onReceiveResult(sf::SFResult* result);
};

} /* namespace sf */
#endif /* SFRESTPLUGIN_H_ */
