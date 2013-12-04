/*
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
