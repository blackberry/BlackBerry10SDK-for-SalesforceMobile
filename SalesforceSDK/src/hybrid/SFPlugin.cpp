/*
 * SFPlugin.cpp
 *
 *  Created on: Dec 1, 2013
 *      Author: timshi
 */

#include "SFPlugin.h"
#include <bb/data/JsonDataAccess>

namespace sf {
using namespace bb::cascades;

SFPlugin::	SFPlugin(WebView* webView){
	mWebView = webView;
}

SFPlugin::~SFPlugin() {
}
void SFPlugin::evalJavascript(const QString& js){
	mWebView->evaluateJavaScript(js);
}

QString SFPlugin::variantToJSON(const QVariant& variant){
	QString json;
	if (!variant.isNull()){
		bb::data::JsonDataAccess jda;
		jda.saveToBuffer(variant, &json);
	}else{
		json = "\"\"";
	}
	return json;
}

void SFPlugin::sendPluginResult(const SFPluginResult& result, const QString& callbackId){
	QString message = variantToJSON(result.getMessage());
	//qDebug()<<"result status is "<< result.getStatus();
	//qDebug()<<"result keep callback is "<< result.isKeepCallback();
	QString keepCallBack = result.isKeepCallback()?"true":"false";
	message = message.isNull()?"":message;
	QString js = QString("sf.executeCallback(\"%1\",%2,%3,%4)").arg(callbackId,QString::number(result.getStatus()),message,keepCallBack);
	qDebug()<<"javascript to be evaled is:  " << js;
	this->evalJavascript(js);
}

} /* namespace sf */
