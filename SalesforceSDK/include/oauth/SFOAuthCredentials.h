/*
 * SFOAuthCredentials.h
 *
 *  Created on: Oct 11, 2013
 *      Author: timshi
 */

#include <QObject>
#include <QString>
#include <QDate>
#include <bb/cascades/WebView>
#include "Serializable.h"

#ifndef SFOAUTHCREDENTIALS_H_
#define SFOAUTHCREDENTIALS_H_

namespace sf {

/*!
 * @class SFOAuthCredentials
 * @headerfile SFOAuthCredentials.h <oauth/SFOAuthCredentials.h>
 *
 * @brief Object representing an individual user account's logon credentials.
 *
 * @details
 * This object represents information about a user account necessary to authenticate and
 * reauthenticate against Salesforce.com servers using OAuth2. It includes information such as
 * the user's account ID, the protocol to use, and any access or refresh tokens assigned by the server.
 *
 * The secure information contained in this object is stored on in the application's data folder after encrypted with AES encryption
 *
 * Instances of this object are used to begin the authentication process, by supplying
 * it to an `SFOAuthCoordinator` instance which conducts the authentication workflow.
 */
class SFOAuthCredentials : public Serializable{
	Q_OBJECT
	Q_PROPERTY(QString clientId READ getClientId WRITE setClientId)
	Q_PROPERTY(QString domain READ getDomain WRITE setDomain)
	Q_PROPERTY(QString redirectUrl READ getRedirectUrl WRITE setRedirectUrl)
	Q_PROPERTY(QString protocol READ getProtocol WRITE setProtocol)
	Q_PROPERTY(QUrl identityUrl READ getIdentityUrl WRITE setIdentityUrl)
	Q_PROPERTY(QUrl instanceUrl READ getInstanceUrl WRITE setInstanceUrl)
	Q_PROPERTY(QString issuedAt READ getIssuedAt WRITE setIssuedAt)
	Q_PROPERTY(QString userId READ getUserId WRITE setUserId)
	Q_PROPERTY(QString identifier READ getIdentifier WRITE setIdentifier)

public:
	SFOAuthCredentials();
	virtual ~SFOAuthCredentials();

	/*
	 * Getters and setters
	 */
	QString getRefreshToken() const;
	void setRefreshToken(QString refreshToken);
	QString getClientId() const;
	void setClientId(QString clientId);
	QString getDomain() const;
	void setDomain(QString domain);
	QString getRedirectUrl() const;
	void setRedirectUrl(QString redirectUrl);
	QString getProtocol() const;
	void setProtocol(QString protocol);
	QString getAccessToken() const;
	void setAccessToken(QString accessToken);
	QUrl getIdentityUrl() const;
	void setIdentityUrl(QUrl identityUrl);
	QUrl getInstanceUrl() const;
	void setInstanceUrl(QUrl instanceUrl);
	QString getIssuedAt();
	void setIssuedAt(QString issuedAt);
	QString getUserId() const;
	void setUserId(QString userId);
	const QString& getIdentifier() const;
	void setIdentifier(const QString& identifier);

	/*!
	 * Removes all tokens from the device.
	 */
	void revoke();
	/*!
	 * Removes the access token
	 */
	void revokeAccessToken();
	/*!
	 * Removes the refresh token
	 */
	void revokeRefreshToken();
	/*!
	 * @param key
	 * Persist the object to file using key.
	 */
	void archiveValueToFile(QString key);
	/*!
	 * @param key
	 * @return true if the object can be deserialized from file using the key
	 */
	bool unArchiveValueFromFile(QString key);
	/*!
	 * @param key
	 * @return true if the credential object is removed from the device successfully
	 */
	static bool removeCredentialArchive(QString key);

private:
	QString mProtocol;
	QString mDomain;
	QString mClientId;
	QString mRedirectUrl;
	QString mActivationCode;
	QUrl mInstanceUrl;
	QString mIssuedAt;
	QUrl mIdentityUrl;
	QString mUserId;
	QString mIdentifier;

	void saveToken(QString token, QString key) const;
	QString retreiveToken(QString key) const;
	QString keyForToken(QString serviceType) const;
};

}
#endif /* SFOAUTHCREDENTIALS_H_ */
