/*
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
    SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onIdReplyReady(sf::SFResult*)));
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
		emit identityCoordinatorDidFailWithError(error);
	} else {
		//normal flow
		QVariantMap reply = result->payload<QVariantMap>();
		handleIdResponse(reply);
	}
	mIsRetrievingData = false;
}


} /* namespace sf */
