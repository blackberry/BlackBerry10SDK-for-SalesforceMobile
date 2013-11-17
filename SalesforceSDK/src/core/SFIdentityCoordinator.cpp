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
* SFIdentityCoordinator.cpp
*
*  Created on: Oct 29, 2013
*      Author: timshi
*/

#include <QtNetwork/QNetworkRequest>
#include <bb/data/JsonDataAccess>
#include "SFIdentityCoordinator.h"
#include "SFNetworkAccessTask.h"
#include "SFGlobal.h"
#include "SFRestAPI.h"
#include "SFOAuthCredentials.h"
#include "SFIdentityData.h"
#include "SFResult.h"
#include "SFRestResourceTask.h"

namespace sf {

static const QString kSFOAuthErroryTypeIdentityRequest = "identity_request_failed";

SFIdentityCoordinator::SFIdentityCoordinator(SFIdentityData* idData) {
	mIsRetrievingData = false;
	mIdData = idData;
	idData->setParent(this);
}

SFIdentityCoordinator::~SFIdentityCoordinator() {
}

void SFIdentityCoordinator::initiateIdentityDataRetrieval(SFOAuthCredentials* credentials){
	mIsRetrievingData = true;
	SFRestRequest* request = SFRestAPI::instance()->customRequest(credentials->getIdentityUrl().toString(), HTTPMethod::HTTPGet);
	SFRestResourceTask *task = new SFRestResourceTask(getSharedNetworkAccessManager(), request);
	task->setCancellable(true);
	connect(this, SIGNAL(cancelIdTask()), task, SLOT(cancel()));
	task->startTaskAsync(this, SLOT(onIdReplyReady(sf::SFResult*)));
}

void SFIdentityCoordinator::cancelRetrieval(){
	emit cancelIdTask();
	mIsRetrievingData = false;
}

SFIdentityData* SFIdentityCoordinator::getIdData(){
	return mIdData;
}
bool SFIdentityCoordinator::isRetrievingData(){
	return mIsRetrievingData;
}


/*
 * private
 */
void SFIdentityCoordinator::handleIdResponse(QVariantMap responseData){
	if (mIdData!=NULL){
		mIdData->deleteLater();
		mIdData=NULL;
	}
	mIdData = new SFIdentityData(responseData);
	emit identityCoordinatorRetrievedData(this);
}

/*
 * slots
 */
void SFIdentityCoordinator::onIdReplyReady(sf::SFResult* result){
	QByteArray replyData;
	if (result->hasError()) {
		sfDebug() << "id request finished with error:" << result->code() << result->message();
		QVariantMap error;
		error.insert(kSFOAuthError, kSFOAuthErroryTypeIdentityRequest);
		error.insert(kSFOAuthErrorDescription, result->message());
		if (mIsRetrievingData){//otherwise the request was canceled, so we don't need to signal error
			emit identityCoordinatorDidFailWithError(error);
		}
	} else {
		//normal flow
		QVariantMap reply = result->payload<QVariantMap>();
		handleIdResponse(reply);
	}
	mIsRetrievingData = false;
}


} /* namespace sf */
