/*
* Copyright 2013 BlackBerry Limited.
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
* SFAbstractApplicationUI.cpp
*
*  Created on: Nov 7, 2013
*      Author: Livan Yi Du
*/

#include "SFAbstractApplicationUI.h"
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <QtDeclarative>
#include "SFAuthenticationManager.h"
#include "SFRestAPI.h"
#include "SFAccountManager.h"

namespace sf {
/* Constants */
QString const SFRemoteAccessConsumerKey = "";
QString const SFOAuthRedirectURI = "";
QString const SFDefaultRestApiVersion = "";

SFAbstractApplicationUI::SFAbstractApplicationUI(bb::cascades::Application *app) : QObject(app) {

	//register meta types for SF classes and enums
	sfRegisterMetaTypes();

	//Setup SFDC connection info
	SFAccountManager::setClientId(SFRemoteAccessConsumerKey);
	SFAccountManager::setRedirectUri(SFOAuthRedirectURI);
	//In addition, you can set up scope for your token like below.
	/*
	QList<QString> scopes;
	scopes.append("api");
	SFAccountManager::setScopes(scopes);
	*/

	//setup API objects
	SFRestAPI::instance()->setApiVersion(SFDefaultRestApiVersion);

	//Expose API objects to QML
	QDeclarativeEngine *engine = QmlDocument::defaultDeclarativeEngine();
	QDeclarativeContext *context = engine ? engine->rootContext() : NULL;
	if (context) {
		context->setContextProperty("SFAccountManager", SFAccountManager::instance());
		context->setContextProperty("SFAuthenticationManager", SFAuthenticationManager::instance());
		context->setContextProperty("SFRestAPI", SFRestAPI::instance());
	} else {
		sfWarning() << "[SFAbstractApplicationUI] Failed to grab shared QML declarative engine. SF APIs may not be accessible in QML.";
	}

	//connect some slots
	connect(app, SIGNAL(aboutToQuit()), SFAuthenticationManager::instance(), SLOT(onAboutToQuit()));
	connect(app, SIGNAL(fullscreen()), SFAuthenticationManager::instance(), SLOT(onAppStart()));
}

SFAbstractApplicationUI::~SFAbstractApplicationUI() {

}

} /* namespace sf */
