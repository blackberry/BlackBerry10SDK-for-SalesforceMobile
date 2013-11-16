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
* SFIdentityData.cpp
*
*  Created on: Oct 29, 2013
*      Author: timshi
*/

#include "SFIdentityData.h"
#include <QMetaObject>
#include <QMetaProperty>

namespace sf {

static QString const kSFIdentityFileDir = "sf_identity_store";

SFIdentityData::SFIdentityData(){
	mUtcOffset = 0;
	mIsActive = false;
	mAssertedUser = false;
}

SFIdentityData::SFIdentityData(QVariantMap map){
	mUtcOffset = 0;
	mIsActive = false;
	mAssertedUser = false;
	deserializeFromMap(map);
}

void SFIdentityData::deserializeFromMap(QVariantMap map){
	const QMetaObject *metaObj = metaObject();
	int count = metaObj->propertyCount();
	for(int i=0; i<count; i++) {
		QMetaProperty property = metaObj->property(i);
		if (!property.isWritable()) {
			continue;
		}
		QVariant value;
		if (map.contains(property.name())){
			value = map[property.name()];
		}else if (map.contains("photos") && map["photos"].toMap().contains(property.name())){
			value = map["photos"].toMap()[property.name()];
		}else if(map.contains("urls") && map["urls"].toMap().contains(property.name())){
			value = map["urls"].toMap()[property.name()];
		}else if(map.contains("mobile_policy") && map["mobile_policy"].toMap().contains(property.name())){
			value = map["mobile_policy"].toMap()[property.name()];
		}else{
			continue;
		}
		if (!const_cast<QVariant&>(value).canConvert(property.type())) {
			continue;
		}
		property.write(this, value);
	}
}

void SFIdentityData::archiveValueToFile(QString key){
	this->archiveValueToFileInDir(key, kSFIdentityFileDir);
}

bool SFIdentityData::unArchiveValueFromFile(QString key){
	return this->unArchiveValueFromFileInDir(key, kSFIdentityFileDir);
}

bool SFIdentityData::removeIdentityArchive(QString key){
	return removeArchiveInDir(key, kSFIdentityFileDir);
}


bool SFIdentityData::isAssertedUser() const {
	return mAssertedUser;
}

void SFIdentityData::setAssertedUser(bool assertedUser) {
	mAssertedUser = assertedUser;
}

const QString& SFIdentityData::getChatterFeedItemsUrl() const {
	return mChatterFeedItemsUrl;
}

void SFIdentityData::setChatterFeedItemsUrl(
		const QString& chatterFeedItemsUrl) {
	mChatterFeedItemsUrl = chatterFeedItemsUrl;
}

const QString& SFIdentityData::getChatterFeedsUrl() const {
	return mChatterFeedsUrl;
}

void SFIdentityData::setChatterFeedsUrl(const QString& chatterFeedsUrl) {
	mChatterFeedsUrl = chatterFeedsUrl;
}

const QString& SFIdentityData::getChatterGroupsUrl() const {
	return mChatterGroupsUrl;
}

void SFIdentityData::setChatterGroupsUrl(const QString& chatterGroupsUrl) {
	mChatterGroupsUrl = chatterGroupsUrl;
}

const QString& SFIdentityData::getChatterUsersUrl() const {
	return mChatterUsersUrl;
}

void SFIdentityData::setChatterUsersUrl(const QString& chatterUsersUrl) {
	mChatterUsersUrl = chatterUsersUrl;
}

const QString& SFIdentityData::getDisplayName() const {
	return mDisplayName;
}

void SFIdentityData::setDisplayName(const QString& displayName) {
	mDisplayName = displayName;
}

const QString& SFIdentityData::getEmail() const {
	return mEmail;
}

void SFIdentityData::setEmail(const QString& email) {
	mEmail = email;
}

const QString& SFIdentityData::getEnterpriseSoapUrl() const {
	return mEnterpriseSoapUrl;
}

void SFIdentityData::setEnterpriseSoapUrl(const QString& enterpriseSoapUrl) {
	mEnterpriseSoapUrl = enterpriseSoapUrl;
}

const QString& SFIdentityData::getFirstName() const {
	return mFirstName;
}

void SFIdentityData::setFirstName(const QString& firstName) {
	mFirstName = firstName;
}

const QUrl& SFIdentityData::getIdUrl() const {
	return mIdUrl;
}

void SFIdentityData::setIdUrl(const QUrl& idUrl) {
	mIdUrl = idUrl;
}

bool SFIdentityData::isIsActive() const {
	return mIsActive;
}

void SFIdentityData::setIsActive(bool isActive) {
	mIsActive = isActive;
}

const QString& SFIdentityData::getLanguage() const {
	return mLanguage;
}

void SFIdentityData::setLanguage(const QString& language) {
	mLanguage = language;
}

const QString& SFIdentityData::getLastModifiedDate() const {
	return mLastModifiedDate;
}

void SFIdentityData::setLastModifiedDate(const QString& lastModifiedDate) {
	mLastModifiedDate = lastModifiedDate;
}

const QString& SFIdentityData::getLastName() const {
	return mLastName;
}

void SFIdentityData::setLastName(const QString& lastName) {
	mLastName = lastName;
}

const QString& SFIdentityData::getLocale() const {
	return mLocale;
}

void SFIdentityData::setLocale(const QString& locale) {
	mLocale = locale;
}

const QString& SFIdentityData::getMetadataSoapUrl() const {
	return mMetadataSoapUrl;
}

void SFIdentityData::setMetadataSoapUrl(const QString& metadataSoapUrl) {
	mMetadataSoapUrl = metadataSoapUrl;
}

const QString& SFIdentityData::getMobileAppPinLength() const {
	return mMobileAppPinLength;
}

void SFIdentityData::setMobileAppPinLength(const QString& mobileAppPinLength) {
	mMobileAppPinLength = mobileAppPinLength;
}

const QString& SFIdentityData::getMobileAppScreenLockTimeout() const {
	return mMobileAppScreenLockTimeout;
}

void SFIdentityData::setMobileAppScreenLockTimeout(
		const QString& mobileAppScreenLockTimeout) {
	mMobileAppScreenLockTimeout = mobileAppScreenLockTimeout;
}

const QString& SFIdentityData::getNickname() const {
	return mNickname;
}

void SFIdentityData::setNickname(const QString& nickname) {
	mNickname = nickname;
}

const QString& SFIdentityData::getOrgId() const {
	return mOrgId;
}

void SFIdentityData::setOrgId(const QString& orgId) {
	mOrgId = orgId;
}

const QString& SFIdentityData::getPartnerSoapUrl() const {
	return mPartnerSoapUrl;
}

void SFIdentityData::setPartnerSoapUrl(const QString& partnerSoapUrl) {
	mPartnerSoapUrl = partnerSoapUrl;
}

const QUrl& SFIdentityData::getPictureUrl() const {
	return mPictureUrl;
}

void SFIdentityData::setPictureUrl(const QUrl& pictureUrl) {
	mPictureUrl = pictureUrl;
}

const QUrl& SFIdentityData::getProfileUrl() const {
	return mProfileUrl;
}

void SFIdentityData::setProfileUrl(const QUrl& profileUrl) {
	mProfileUrl = profileUrl;
}

const QString& SFIdentityData::getRestQueryUrl() const {
	return mRestQueryUrl;
}

void SFIdentityData::setRestQueryUrl(const QString& restQueryUrl) {
	mRestQueryUrl = restQueryUrl;
}

const QString& SFIdentityData::getRestRecentUrl() const {
	return mRestRecentUrl;
}

void SFIdentityData::setRestRecentUrl(const QString& restRecentUrl) {
	mRestRecentUrl = restRecentUrl;
}

const QString& SFIdentityData::getRestSearchUrl() const {
	return mRestSearchUrl;
}

void SFIdentityData::setRestSearchUrl(const QString& restSearchUrl) {
	mRestSearchUrl = restSearchUrl;
}

const QString& SFIdentityData::getRestSObjectsUrl() const {
	return mRestSObjectsUrl;
}

void SFIdentityData::setRestSObjectsUrl(const QString& restSObjectsUrl) {
	mRestSObjectsUrl = restSObjectsUrl;
}

const QString& SFIdentityData::getRestUrl() const {
	return mRestUrl;
}

void SFIdentityData::setRestUrl(const QString& restUrl) {
	mRestUrl = restUrl;
}

const QUrl& SFIdentityData::getThumbnailUrl() const {
	return mThumbnailUrl;
}

void SFIdentityData::setThumbnailUrl(const QUrl& thumbnailUrl) {
	mThumbnailUrl = thumbnailUrl;
}

const QString& SFIdentityData::getUserId() const {
	return mUserId;
}

void SFIdentityData::setUserId(const QString& userId) {
	mUserId = userId;
}

const QString& SFIdentityData::getUsername() const {
	return mUsername;
}

void SFIdentityData::setUsername(const QString& username) {
	mUsername = username;
}

const QString& SFIdentityData::getUserType() const {
	return mUserType;
}

void SFIdentityData::setUserType(const QString& userType) {
	mUserType = userType;
}

int SFIdentityData::getUtcOffset() const {
	return mUtcOffset;
}

void SFIdentityData::setUtcOffset(int utcOffset) {
	mUtcOffset = utcOffset;
}

SFIdentityData::~SFIdentityData() {
}
} /* namespace sf */
