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

class SFRestPlugin : public SFPlugin{
	Q_OBJECT
public:
	SFRestPlugin(WebView* webView);
	virtual ~SFRestPlugin();
	Q_INVOKABLE
	void sendCustomRequest(const SFInvokedUrlCommand& command); //custom rest request
	Q_INVOKABLE
	void sendRequestForVersions(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void sendRequestForResources(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void sendRequestForDescribeGlobal(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void sendRequestForMetadata(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void sendRequestForDescribeObject(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void sendRequestForRetrieveObject(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void sendRequestForCreateObject(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void sendRequestForUpdateObject(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void sendRequestForUpsertObject(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void sendRequestForDeleteObject(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void sendRequestForQuery(const SFInvokedUrlCommand& command);
	Q_INVOKABLE
	void sendRequestForSearch(const SFInvokedUrlCommand& command);

private slots:
	void onReceiveResult(sf::SFResult* result);
};

} /* namespace sf */
#endif /* SFRESTPLUGIN_H_ */
