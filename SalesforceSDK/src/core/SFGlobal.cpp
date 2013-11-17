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
