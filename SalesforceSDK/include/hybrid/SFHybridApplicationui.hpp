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

/*!
 * @class SFHybridApplicationUI
 * @headerfile SFHybridApplicationUI.h "SFHybridApplicationUI.h"
 * @brief Abstract base class for the main application UI class in a cascade html5 hybrid application
 *
 * @details
 * This abstract class's constructor contains the minimum plumbing required to setting up
 * the application to be able to running html5 applications in a web view while using the
 * native authentication and REST API methods in the SDK.
 *
 * Usage
 * -----
 * Create a concrete subclass of this class.
 * @code{.cpp}
 *   ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
 *   	sf::SFAbstractApplicationUI(app)
 *   	{
 *   		//application specific setup
 *
 *   		//standard way of loading the main UI.
 *   		QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
 *   		AbstractPane *root = qml->createRootObject<AbstractPane>();
 *   		app->setScene(root);
 *
 *			//call these two methods to setup the web view and default native plugins
 *    		this->setupWebView(root);
 *   		this->registerDefaultPlugins();
 *   	}
 * @endcode
 * The main qml should contain the SFHybridWebView
 * @code{.qml}
 * 	import bb.cascades 1.2
 * 	import "./salesforce"
 * 	Page {
 *  	objectName: "page";
 *  	SFHybridWebView{}
 * 	}
 * @endcode
 *
 * \sa SFAuthPlugin, SFRestPlugin, SFPlugin, SFHybridWebView.qml, sf.js
 *
 * \author Tim Shi
 */
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
    /*!
     * After a plugin is added, it can be invoked by from javascript running on the web view
     * @param plugin The plugin to be added to the list of plugins associated with the web view
     */
    void addPlugin(SFPlugin* plugin);
    /*!
     * Establishes reference for mWebView, and connects the web view's signal to slots defined on this class
     * @param root The root of the application UI.
     */
    void setupWebView(AbstractPane* root);
    /*!
     * Instantiates and adds the SFAuthPlugin and SFRestPlugin
     */
    void registerDefaultPlugins();
protected slots:
	/*!
	 * Handles message from javascript when navigator.cascade.postMessage is called
	 * The message is expected to be in the form of
	 * {
	 * 	callbackId:"",
	 * 	className:"",
	 * 	methodName:"",
	 * 	arguments:{} //key value pairs representing each argument and its value.
	 * }
	 * The callbackId is the id of the callback function defined in javascript (See @c sf.js).
	 * The className is the fully qualified name of the plugin class.
	 * The methodName is the name of an invokable method on the plugin class.
	 * The arguments is the arguments passed to that method
	 * @param message The message received from javascript when navigator.cascade.postMessage is called
	 */
	void onMessageReceived(const QVariantMap& message);
private:
	/*!
	 * Helper method to allow the native code to send an error message back to javascript code if there is an error
	 * executing the native plugin from javascript.
	 * @param callbackId The callbackId for the javascript function
	 * @param status The error status
	 */
	void sendErrorToJs(QString callbackId, SFPluginCommandStatus status);
};
} /* namespace sf */
#endif /* SFHybridApplicationUI_HPP_ */
