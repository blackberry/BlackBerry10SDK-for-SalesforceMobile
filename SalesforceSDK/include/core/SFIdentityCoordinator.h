/*
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

/*
 * retrieves user data based on credentials.
 * caller of this class is responsible for refreshing id data when credentials changed.
 */
class SFIdentityCoordinator : public QObject{
	Q_OBJECT

private:
	bool mIsRetrievingData;
	SFIdentityData* mIdData;

public:
	SFIdentityCoordinator(SFIdentityData* idData);
	virtual ~SFIdentityCoordinator();

	void initiateIdentityDataRetrieval(SFOAuthCredentials* credentials);
	void cancelRetrieval();
	SFIdentityData* getIdData();
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
