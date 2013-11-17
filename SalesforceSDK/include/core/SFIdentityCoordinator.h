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
* SFIdentityCoordinator.h
*
*  Created on: Oct 29, 2013
*      Author: timshi
*/

#ifndef SFIDENTITYCOORDINATOR_H_
#define SFIDENTITYCOORDINATOR_H_

#include <QObject>
#include <QVariantMap>

namespace sf {

class SFOAuthCredentials;
class SFIdentityData;
class SFResult;

/*!
 * @class SFIdentityCoordinator
 * @headerfile SFIdentityCoordinator.h <core/SFIdentityCooridnator.h>
 * @brief The SFIdentityCoordinator class is used to retrieve identity data from the ID endpoint of the
 * Salesforce service. This class is not meant to be used by the SDK classes only.
 *
 * @details
 * The SFIdentityCoordinator class is used to retrieve identity data from the ID endpoint of the
 * Salesforce service.  This data will be based on the requesting user, and the OAuth app
 * credentials he/she is using to request this information. This class is not meant to be used by the SDK classes only.
 */
class SFIdentityCoordinator : public QObject{
	Q_OBJECT

private:
	bool mIsRetrievingData;
	SFIdentityData* mIdData;

public:
	SFIdentityCoordinator(SFIdentityData* idData);
	virtual ~SFIdentityCoordinator();

	/*!
	 * @param credentials Begins the identity request using the credentials
	 */
	void initiateIdentityDataRetrieval(SFOAuthCredentials* credentials);
	/*!
	 * Cancels the id retrieval
	 */
	void cancelRetrieval();
	/*!
	 * @return The SFIdentityData that will be populated with the response data from the service.
	 */
	SFIdentityData* getIdData();
	/*!
	 * @return whether the coordinator is in the process of retreiving identity data
	 */
	bool isRetrievingData();
signals:
	void identityCoordinatorRetrievedData(SFIdentityCoordinator* coordinator);
	void identityCoordinatorDidFailWithError(QVariantMap error);
	void cancelIdTask();

private:
	void handleIdResponse(QVariantMap responseData);

private slots:
	void onIdReplyReady(sf::SFResult* result);
};

} /* namespace sf */
#endif /* SFIDENTITYCOORDINATOR_H_ */
