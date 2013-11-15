/*
 * SFIdentityData.h
 *
 *  Created on: Oct 29, 2013
 *      Author: timshi
 */

#ifndef SFIDENTITYDATA_H_
#define SFIDENTITYDATA_H_

#include "Serializable.h"
#include <QVariant>
#include <QVariantMap>
#include <QUrl>
namespace sf {

/*!
 * @class SFIdentityData
 * @headerfile SFIdentityData.h <core/SFIdentityData.h>
 * @brief The data structure for the identity data that's retrieved from the Salesforce service.
 */
class SFIdentityData : public Serializable{
	Q_OBJECT
	Q_PROPERTY(QUrl id READ getIdUrl WRITE setIdUrl)
	Q_PROPERTY(bool asserted_user READ isAssertedUser WRITE setAssertedUser)
	Q_PROPERTY(QString user_id READ getUserId WRITE setUserId)
	Q_PROPERTY(QString organization_id READ getOrgId WRITE setOrgId)
	Q_PROPERTY(QString username READ getUsername WRITE setUsername)
	Q_PROPERTY(QString nick_name READ getNickname WRITE setNickname)
	Q_PROPERTY(QString display_name READ getDisplayName WRITE setDisplayName)
	Q_PROPERTY(QString email READ getEmail WRITE setEmail)
	Q_PROPERTY(QString first_name READ getFirstName WRITE setFirstName)
	Q_PROPERTY(QString last_name READ getLastName WRITE setLastName)
	/*
	 * when looking these two properties up from the map, they belong in a parent node called "photo"
	 */
	Q_PROPERTY(QUrl picture READ getPictureUrl WRITE setPictureUrl)
	Q_PROPERTY(QUrl thumbnail READ getThumbnailUrl WRITE setThumbnailUrl)
	/*
	 * belongs under a urls parent
	 */
	Q_PROPERTY(QString enterprise READ getEnterpriseSoapUrl WRITE setEnterpriseSoapUrl)
	Q_PROPERTY(QString metadata READ getMetadataSoapUrl WRITE setMetadataSoapUrl)
	Q_PROPERTY(QString partner READ getPartnerSoapUrl WRITE setPartnerSoapUrl)
	Q_PROPERTY(QString rest READ getRestUrl WRITE setRestUrl)
	Q_PROPERTY(QString sobjects READ getRestSObjectsUrl WRITE setRestSObjectsUrl)
	Q_PROPERTY(QString search READ getRestSearchUrl WRITE setRestSearchUrl)
	Q_PROPERTY(QString query READ getRestQueryUrl WRITE setRestQueryUrl)
	Q_PROPERTY(QString recent READ getRestRecentUrl WRITE setRestRecentUrl)
	Q_PROPERTY(QUrl profile READ getProfileUrl WRITE setProfileUrl)
	Q_PROPERTY(QString feeds READ getChatterFeedsUrl WRITE setChatterFeedsUrl)
	Q_PROPERTY(QString groups READ getChatterGroupsUrl WRITE setChatterGroupsUrl)
	Q_PROPERTY(QString users READ getChatterUsersUrl WRITE setChatterUsersUrl)
	Q_PROPERTY(QString feed_items READ getChatterFeedItemsUrl WRITE setChatterFeedItemsUrl)

	Q_PROPERTY(bool active READ isIsActive WRITE setIsActive)
	Q_PROPERTY(QString user_type READ getUserType WRITE setUserType)
	Q_PROPERTY(QString language READ getLanguage WRITE setLanguage)
	Q_PROPERTY(QString locale READ getLocale WRITE setLocale)
	Q_PROPERTY(int utcOffset READ getUtcOffset WRITE setUtcOffset)
	Q_PROPERTY(QString pin_length READ getMobileAppPinLength WRITE setMobileAppPinLength)
	Q_PROPERTY(QString screen_lock READ getMobileAppScreenLockTimeout WRITE setMobileAppScreenLockTimeout)
	Q_PROPERTY(QString last_modified_date READ getLastModifiedDate WRITE setLastModifiedDate)

private:
	QUrl mIdUrl;
	bool mAssertedUser;
	QString mUserId;
	QString mOrgId;
	QString mUsername;
	QString mNickname;
	QString mDisplayName;
	QString mEmail;
	QString mFirstName;
	QString mLastName;
	QUrl mPictureUrl;
	QUrl mThumbnailUrl;
	QString mEnterpriseSoapUrl;
	QString mMetadataSoapUrl;
	QString mPartnerSoapUrl;
	QString mRestUrl;
	QString mRestSObjectsUrl;
	QString mRestSearchUrl;
	QString mRestQueryUrl;
	QString mRestRecentUrl;
	QUrl mProfileUrl;
	QString mChatterFeedsUrl;
	QString mChatterGroupsUrl;
	QString mChatterUsersUrl;
	QString mChatterFeedItemsUrl;
	bool mIsActive;
	QString mUserType;
	QString mLanguage;
	QString mLocale;
	int mUtcOffset;
	QString mMobileAppPinLength;
	QString mMobileAppScreenLockTimeout;
	QString mLastModifiedDate;

public:
	SFIdentityData();
	SFIdentityData(QVariantMap map);
	virtual ~SFIdentityData();
	bool isAssertedUser() const;
	void setAssertedUser(bool assertedUser);
	const QString& getChatterFeedItemsUrl() const;
	void setChatterFeedItemsUrl(const QString& chatterFeedItemsUrl);
	const QString& getChatterFeedsUrl() const;
	void setChatterFeedsUrl(const QString& chatterFeedsUrl);
	const QString& getChatterGroupsUrl() const;
	void setChatterGroupsUrl(const QString& chatterGroupsUrl);
	const QString& getChatterUsersUrl() const;
	void setChatterUsersUrl(const QString& chatterUsersUrl);
	const QString& getDisplayName() const;
	void setDisplayName(const QString& displayName);
	const QString& getEmail() const;
	void setEmail(const QString& email);
	const QString& getEnterpriseSoapUrl() const;
	void setEnterpriseSoapUrl(const QString& enterpriseSoapUrl);
	const QString& getFirstName() const;
	void setFirstName(const QString& firstName);
	const QUrl& getIdUrl() const;
	void setIdUrl(const QUrl& idUrl);
	bool isIsActive() const;
	void setIsActive(bool isActive);
	const QString& getLanguage() const;
	void setLanguage(const QString& language);
	const QString& getLastModifiedDate() const;
	void setLastModifiedDate(const QString& lastModifiedDate);
	const QString& getLastName() const;
	void setLastName(const QString& lastName);
	const QString& getLocale() const;
	void setLocale(const QString& locale);
	const QString& getMetadataSoapUrl() const;
	void setMetadataSoapUrl(const QString& metadataSoapUrl);
	const QString& getMobileAppPinLength() const;
	void setMobileAppPinLength(const QString& mobileAppPinLength);
	const QString& getMobileAppScreenLockTimeout() const;
	void setMobileAppScreenLockTimeout(const QString& mobileAppScreenLockTimeout);
	bool isMobilePoliciesConfigured() const;
	void setMobilePoliciesConfigured(bool mobilePoliciesConfigured);
	const QString& getNickname() const;
	void setNickname(const QString& nickname);
	const QString& getOrgId() const;
	void setOrgId(const QString& orgId);
	const QString& getPartnerSoapUrl() const;
	void setPartnerSoapUrl(const QString& partnerSoapUrl);
	const QUrl& getPictureUrl() const;
	void setPictureUrl(const QUrl& pictureUrl);
	const QUrl& getProfileUrl() const;
	void setProfileUrl(const QUrl& profileUrl);
	const QString& getRestQueryUrl() const;
	void setRestQueryUrl(const QString& restQueryUrl);
	const QString& getRestRecentUrl() const;
	void setRestRecentUrl(const QString& restRecentUrl);
	const QString& getRestSearchUrl() const;
	void setRestSearchUrl(const QString& restSearchUrl);
	const QString& getRestSObjectsUrl() const;
	void setRestSObjectsUrl(const QString& restSObjectsUrl);
	const QString& getRestUrl() const;
	void setRestUrl(const QString& restUrl);
	const QUrl& getThumbnailUrl() const;
	void setThumbnailUrl(const QUrl& thumbnailUrl);
	const QString& getUserId() const;
	void setUserId(const QString& userId);
	const QString& getUsername() const;
	void setUsername(const QString& username);
	const QString& getUserType() const;
	void setUserType(const QString& userType);
	int getUtcOffset() const;
	void setUtcOffset(int utcOffset);

	void archiveValueToFile(QString key);
	bool unArchiveValueFromFile(QString key);

	static bool removeIdentityArchive(QString key);
private:
	void deserializeFromMap(QVariantMap map);
};

} /* namespace sf */
#endif /* SFIDENTITYDATA_H_ */
