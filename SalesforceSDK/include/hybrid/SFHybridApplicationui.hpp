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

#ifndef SFHybridApplicationUI_HPP_
#define SFHybridApplicationUI_HPP_

#include <QObject>
#include <QList>
#include <bb/cascades/Application>
#include <bb/cascades/WebView>
#include "SFAuthPlugin.h"
#include "SFPlugin.h"
#include "SFPluginResult.h"

namespace sf {

/*
 * Subclass should define following constants as basic application setup. Otherwise you will receive complier error
 */
extern QString const SFRemoteAccessConsumerKey;
extern QString const SFOAuthRedirectURI;
extern QString const SFDefaultRestApiVersion;

class SFHybridApplicationUI : public QObject
{
    Q_OBJECT
public:
    SFHybridApplicationUI(bb::cascades::Application *app);
    virtual ~SFHybridApplicationUI() { }
protected:
    bb::cascades::WebView* mWebView;
    QList<SFPlugin*> mPlugins;
protected:
    void addPlugin(SFPlugin* plugin);
protected slots:
	void onMessageReceived(const QVariantMap& message);
private:
	void sendErrorToJs(QString callbackId, SFPluginCommandStatus status);
};
} /* namespace sf */
#endif /* SFHybridApplicationUI_HPP_ */
