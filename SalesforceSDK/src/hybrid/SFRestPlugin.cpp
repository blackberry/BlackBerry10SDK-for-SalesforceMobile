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
 * SFRestPlugin.cpp
 *
 *  Created on: Dec 2, 2013
 *      Author: timshi
 */

#include "SFRestPlugin.h"
#include "SFRestAPI.h"
#include "SFResult.h"
#include "SFRestRequest.h"

namespace sf {

static QString const kPluginRestPath = "path";
static QString const kPluginRestContentType = "contentType";
static QString const kPluginRestMethod = "method";
static QString const kPluginRestParams = "params";
static QString const kArgObjectType = "objectType";
static QString const kArgObjectId = "objectId";
static QString const kArgFields = "fields";
static QString const kArgExtIdField = "extIdField";
static QString const kArgExtIdValue = "extIdValue";
static QString const kArgSoql = "soql";
static QString const kArgSosl = "sosl";

SFRestPlugin::SFRestPlugin(WebView* webView):SFPlugin(webView) {
}

SFRestPlugin::~SFRestPlugin() {
}

void SFRestPlugin::sendCustomRequest(const SFInvokedUrlCommand& command){
	sfDebug()<<"sendRestRequest "<<command.getArguments();
	QString path = command.getArguments().value(kPluginRestPath).toString();
	QVariantMap params = command.getArguments().value(kPluginRestParams).toMap();
	bool hasMethod;
	int httpMethod = command.getArguments().value(kPluginRestMethod).toInt(&hasMethod);
	if (!hasMethod){
		httpMethod = HTTPMethod::HTTPGet;
	}
	bool hasContentType;
	int contentType = command.getArguments().value(kPluginRestContentType).toInt(&hasContentType);
	if (!hasContentType){
		contentType = sf::SFRestRequest::HTTPContentTypeJSON;
	}
	SFRestRequest* request = SFRestAPI::instance()->customRequest(path, httpMethod, params, contentType);
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::sendRequestForVersions(const SFInvokedUrlCommand& command){
	SFRestRequest* request = SFRestAPI::instance()->requestForVersions();
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::sendRequestForResources(const SFInvokedUrlCommand& command){
	SFRestRequest* request = SFRestAPI::instance()->requestForResources();
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::sendRequestForDescribeGlobal(const SFInvokedUrlCommand& command){
	SFRestRequest* request = SFRestAPI::instance()->requestForDescribeGlobal();
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::sendRequestForMetadata(const SFInvokedUrlCommand& command){
	QString objectType = command.getArguments().value(kArgObjectType).toString();
	SFRestRequest* request = SFRestAPI::instance()->requestForMetadata(objectType);
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::sendRequestForDescribeObject(const SFInvokedUrlCommand& command){
	QString objectType = command.getArguments().value(kArgObjectType).toString();
	SFRestRequest* request = SFRestAPI::instance()->requestForDescribeObject(objectType);
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::sendRequestForRetrieveObject(const SFInvokedUrlCommand& command){
	sfDebug()<<"sendRestRequest "<<command.getArguments();

	QString objectType = command.getArguments().value(kArgObjectType).toString();
	QString objectId = command.getArguments().value(kArgObjectId).toString();
	QList<QVariant> variantList = command.getArguments().value(kArgFields).toList();
	sfDebug()<<"variant list" << variantList;
	QList<QString> objectList;
	QListIterator<QVariant> i(variantList);
	while (i.hasNext()) {
	  objectList << i.next().toString();
	}
	SFRestRequest* request = SFRestAPI::instance()->requestForRetrieveObject(objectType, objectId, objectList);
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::sendRequestForCreateObject(const SFInvokedUrlCommand& command){
	QString objectType = command.getArguments().value(kArgObjectType).toString();
	QVariantMap fields = command.getArguments().value(kArgFields).toMap();
	SFRestRequest* request = SFRestAPI::instance()->requestForCreateObject(objectType, fields);
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::sendRequestForUpdateObject(const SFInvokedUrlCommand& command){
	QString objectType = command.getArguments().value(kArgObjectType).toString();
	QString objectId = command.getArguments().value(kArgObjectId).toString();
	QVariantMap fields = command.getArguments().value(kArgFields).toMap();
	SFRestRequest* request = SFRestAPI::instance()->requestForUpdateObject(objectType, objectId, fields);
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::sendRequestForUpsertObject(const SFInvokedUrlCommand& command){
	QString objectType = command.getArguments().value(kArgObjectType).toString();
	QString extIdField = command.getArguments().value(kArgExtIdField).toString();
	QString extIdValue = command.getArguments().value(kArgExtIdValue).toString();
	QVariantMap fields = command.getArguments().value(kArgFields).toMap();
	SFRestRequest* request = SFRestAPI::instance()->requestForUpsertObject(objectType, extIdField, extIdValue, fields);
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::sendRequestForDeleteObject(const SFInvokedUrlCommand& command){
	QString objectType = command.getArguments().value(kArgObjectType).toString();
	QString objectId = command.getArguments().value(kArgObjectId).toString();
	SFRestRequest* request = SFRestAPI::instance()->requestForDeleteObject(objectType, objectId);
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::sendRequestForQuery(const SFInvokedUrlCommand& command){
	QString soql = command.getArguments().value(kArgSoql).toString();
	SFRestRequest* request = SFRestAPI::instance()->requestForQuery(soql);
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::sendRequestForSearch(const SFInvokedUrlCommand& command){
	QString sosl = command.getArguments().value(kArgSosl).toString();
	SFRestRequest* request = SFRestAPI::instance()->requestForSearch(sosl);
	SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onReceiveResult(sf::SFResult*)), command.getCallbackId());
}
void SFRestPlugin::onReceiveResult(sf::SFResult* result){
	sfDebug()<<"received result " << result->getTag<QString>(kSFRestRequestTag);
	QString callbackId = result->getTag<QString>(kSFRestRequestTag);
	if (!result->hasError()){
		QVariant payload = result->payload<QVariant>();
		sfDebug()<<"sending payload back" << payload;
		SFPluginResult pluginResult = SFPluginResult(SFCommandStatus_OK, payload, false);
		this->sendPluginResult(pluginResult, callbackId);
	}else{
		QVariant payload = result->payload<QVariant>();
		sfDebug()<<"error " << payload;
		SFPluginResult pluginResult = SFPluginResult(SFCommandStatus_ERROR, payload, false);
		this->sendPluginResult(pluginResult, callbackId);
	}
}
} /* namespace sf */
