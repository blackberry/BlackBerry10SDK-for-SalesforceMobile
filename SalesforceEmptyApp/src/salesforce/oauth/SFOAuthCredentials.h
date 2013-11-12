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

	//interfaces
	void revoke();
	void revokeAccessToken();
	void revokeRefreshToken();

	void archiveValueToFile(QString key);
	bool unArchiveValueFromFile(QString key);

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
