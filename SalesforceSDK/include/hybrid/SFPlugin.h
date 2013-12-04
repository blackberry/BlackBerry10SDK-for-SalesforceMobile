/*
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
class SFPlugin : public QObject{
	Q_OBJECT
public:
	SFPlugin(WebView* webView);
	virtual ~SFPlugin();

protected:
	WebView* mWebView;

protected:
	void evalJavascript(const QString& js);
	QString variantToJSON(const QVariant& map);
	void sendPluginResult(const SFPluginResult& result, const QString& callbackId);
};

} /* namespace sf */
#endif /* SFPLUGIN_H_ */
