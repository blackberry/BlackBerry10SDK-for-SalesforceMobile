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
 */

#include "SFHybridApplicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/WebView>
#include <bb/cascades/Container>
#include <bb/cascades/Page>
#include <bb/data/JsonDataAccess>
#include <SFInvokedUrlCommand.h>
#include <QMetaObject>
#include <QMetaMethod>
#include <QString>
#include <SFAccountManager.h>
#include <SFAuthenticationManager.h>
#include "SFRestAPI.h"
#include "SFRestPlugin.h"

namespace sf {

static const QString kSFPluginCommandArguments = "arguments";
static const QString kSFPluginCommandCallbackId = "callbackId";
static const QString kSFPluginCommandClassName = "className";
static const QString kSFPluginCommandMethodName = "methodName";

SFHybridApplicationUI::SFHybridApplicationUI(bb::cascades::Application *app) :
		SFAbstractApplicationUI(app)
{
	mWebView = NULL; //expect the subclass to call setupWebView
}

void SFHybridApplicationUI::setupWebView(AbstractPane* root){
    mWebView = root->findChild<WebView*>("webView");
    connect(mWebView, SIGNAL(messageReceived(QVariantMap)), this, SLOT(onMessageReceived(QVariantMap)));
}

void SFHybridApplicationUI::registerDefaultPlugins(){
    SFAuthPlugin* authPlugin = new SFAuthPlugin(mWebView);
    authPlugin->setParent(this);
    this->addPlugin(authPlugin);

    SFRestPlugin* restPlugin = new SFRestPlugin(mWebView);
    restPlugin->setParent(this);
    this->addPlugin(restPlugin);
}

/*
 * the message should contain:
 * 1. callbackId
 * 2. className
 * 3. methodName
 * 4. map of arguments
 *
 * {
 * 	callbackId:"",
 * 	className:"",
 * 	methodName:"",
 * 	arguments:["", "", ""]
 * }
 */
void SFHybridApplicationUI::onMessageReceived(const QVariantMap& message){
	qDebug()<<"message received " << message.value("data").toString();
	bb::data::JsonDataAccess jda;
	QVariant jsonData = jda.loadFromBuffer(message.value("data").toString().toUtf8());
	if (jda.hasError()){
		qDebug()<<"error is " << jda.error();
	}
	//qDebug()<<"jsonData is " << jsonData;
	QVariantMap data = jsonData.toMap();
	//qDebug()<<"data is " << data;
	SFInvokedUrlCommand command;
	command.setArguments(data.value(kSFPluginCommandArguments).toMap());
	command.setCallbackId(data.value(kSFPluginCommandCallbackId).toString());
	command.setClassName(data.value(kSFPluginCommandClassName).toString());
	command.setMethodName(data.value(kSFPluginCommandMethodName).toString());
	//qDebug()<<"className " << command.getClassName() << "; methodName " << command.getMethodName();

	bool pluginFound = false;
	for (int i = 0; i < mPlugins.size(); i++){
		SFPlugin* plugin = mPlugins[i];
		const QMetaObject* metaObject = plugin->metaObject();
		if (metaObject->className() == command.getClassName()){
			qDebug()<<"class name match";
			for (int i = 0; i < metaObject->methodCount(); i++){
				QMetaMethod m = metaObject->method(i);
				//qDebug()<<"metaMethod is " << m.signature();
				if (QString(m.signature()).startsWith(command.getMethodName())){
					m.invoke(plugin, Qt::DirectConnection, Q_ARG(SFInvokedUrlCommand, command));
					pluginFound = true;
					break;
				}
			}
		}
		if (pluginFound){
			break;
		}
	}

	if (!pluginFound){
		qDebug()<<"plugin not found";
		QString callbackId = command.getCallbackId();
		this->sendErrorToJs(callbackId, SFCommandStatus_CLASS_NOT_FOUND_EXCEPTION);
	}
}

void SFHybridApplicationUI::addPlugin(SFPlugin* plugin){
	mPlugins.append(plugin);
}

void SFHybridApplicationUI::sendErrorToJs(QString callbackId, SFPluginCommandStatus status){
	//TODO: match status with meaningful error message
	QString js = QString("sf.executeCallback(\"%1\",%2,%3,%4)").arg(callbackId,QString::number(status),"\"Plugin Execution Error\"","false");
	qDebug()<<"js is " << js;
	mWebView->evaluateJavaScript(js);
}

} /* namespace sf */
