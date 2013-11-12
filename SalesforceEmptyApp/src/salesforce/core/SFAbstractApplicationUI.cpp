/*
e * SFAbstractApplicationUI.cpp
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
