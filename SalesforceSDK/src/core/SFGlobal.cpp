/*
 * Constants.cpp
 *
 *  Created on: Oct 18, 2013
 *      Author: Livan Yi Du
 */

#include "SFGlobal.h"
#include <QtDeclarative>
#include <QtNetwork/QNetworkAccessManager>
#include "SFRestRequest.h"
#include "SFResult.h"

namespace sf {

const QString SFMobileSDKVersion = "1.0";
const QString SFMobileSDKNativeDesignator = "Native";
const QString kSFRestRequestTag = "SFRestRequestTag";
const QString kSFOAuthError = "error";
const QString kSFOAuthErrorDescription = "error_description";

QNetworkAccessManager* sharedNetworkAccessManager = NULL;

/*
 * Global Function
 */

void sfRegisterMetaTypes() {
	//register enum
	qmlRegisterUncreatableType<HTTPMethod>("sf", 1, 0, "HTTPMethod", "Enum wrapper class");
	qmlRegisterUncreatableType<SFResultCode>("sf", 1, 0, "SFResultCode", "Enum wrapper class");

	//register classes
	qmlRegisterType<SFRestRequest>("sf", 1, 0, "SFRestRequest");
	qmlRegisterType<SFResult>("sf", 1, 0, "SFResult");
}

QNetworkAccessManager* getSharedNetworkAccessManager(){
	if (sharedNetworkAccessManager==NULL){
		sharedNetworkAccessManager = new QNetworkAccessManager();
		//TODO: do additional set up here.
	}
	return sharedNetworkAccessManager;
}

} /* namespace sf */
