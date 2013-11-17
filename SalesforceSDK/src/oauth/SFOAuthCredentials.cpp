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
* SFOAuthCredentials.cpp
*
*  Created on: Oct 11, 2013
*      Author: timshi
*/

#include "SFOAuthCredentials.h"
#include <QDir>
#include <QFile>
#include <QTextIStream>
#include "SFSecurityManager.h"

namespace sf {

static QString const kSFOAuthProtocolHttps = "https";
static QString const kSFOAuthDefaultDomain = "login.salesforce.com";
static QString const kSFOAuthServiceAccess = "com.salesforce.oauth.access";
static QString const kSFOAuthServiceRefresh = "com.salesforce.oauth.refresh";
static QString const kSFTokenFileDir = "sf_token_store";
static QString const kSFCredentialFileDir = "sf_credential_store";

SFOAuthCredentials::SFOAuthCredentials(){
	mProtocol = kSFOAuthProtocolHttps;
	mDomain = kSFOAuthDefaultDomain;
}

SFOAuthCredentials::~SFOAuthCredentials() {
}

QString SFOAuthCredentials::getRefreshToken() const{
	return retreiveToken(keyForToken(kSFOAuthServiceRefresh));
}

void SFOAuthCredentials::setRefreshToken(QString refreshToken) {
	saveToken(refreshToken, keyForToken(kSFOAuthServiceRefresh));
}

QString SFOAuthCredentials::getClientId() const {
	return mClientId;
}

void SFOAuthCredentials::setClientId(QString clientId) {
	mClientId = clientId;
}

QString SFOAuthCredentials::getDomain() const {
	return mDomain;
}

void SFOAuthCredentials::setDomain(QString domain) {
	mDomain = domain;
}

QString SFOAuthCredentials::getRedirectUrl() const {
	return mRedirectUrl;
}

void SFOAuthCredentials::setRedirectUrl(QString redirectUrl) {
	mRedirectUrl = redirectUrl;
}

QString SFOAuthCredentials::getProtocol() const {
	return mProtocol;
}

QString SFOAuthCredentials::getAccessToken() const {
	return retreiveToken(keyForToken(kSFOAuthServiceAccess));
}

void SFOAuthCredentials::setAccessToken(QString accessToken) {
	saveToken(accessToken,keyForToken(kSFOAuthServiceAccess));
}

QUrl SFOAuthCredentials::getIdentityUrl() const {
	return mIdentityUrl;
}

void SFOAuthCredentials::setIdentityUrl(QUrl identityUrl) {
	mIdentityUrl = identityUrl;
}

QUrl SFOAuthCredentials::getInstanceUrl() const {
	return mInstanceUrl;
}

void SFOAuthCredentials::setInstanceUrl(QUrl instanceUrl) {
	mInstanceUrl = instanceUrl;
}

QString SFOAuthCredentials::getIssuedAt(){
	return mIssuedAt;
}

void SFOAuthCredentials::setIssuedAt(QString issuedAt) {
	mIssuedAt = issuedAt;
}

QString SFOAuthCredentials::getUserId() const {
	return mUserId;
}

void SFOAuthCredentials::setUserId(QString userId) {
	mUserId = userId;
}

void SFOAuthCredentials::setProtocol(QString protocol) {
	mProtocol = protocol;
}

const QString& SFOAuthCredentials::getIdentifier() const {
	return mIdentifier;
}

void SFOAuthCredentials::setIdentifier(const QString& identifier) {
	mIdentifier = identifier;
}

void SFOAuthCredentials::revoke(){
	revokeAccessToken();
	revokeRefreshToken();
}
void SFOAuthCredentials::revokeAccessToken(){
	setAccessToken(NULL);
}
void SFOAuthCredentials::revokeRefreshToken(){
	setRefreshToken(NULL);
	mInstanceUrl.clear();
	mIdentityUrl.clear();
	mIssuedAt=QString();
}

void SFOAuthCredentials::saveToken(QString token, QString key) const{
	//qDebug()<<"setting token to "<< token;
	QString encryptedToken = SFSecurityManager::instance()->encrypt(token);
	//qDebug()<<"token encrypted to "<< encryptedToken;

	QDir home = QDir::home();
	QString absoluteDirPath = home.absoluteFilePath(kSFTokenFileDir);
	QDir dir = QDir(absoluteDirPath);
	if (!dir.exists()){
		home.mkdir(kSFTokenFileDir);
	}

	QString path = QString("%1/%2").arg(kSFTokenFileDir, key);
	QString absolutePath = home.absoluteFilePath(path);

	//qDebug()<<"token file path is "<<absolutePath;

	QFile tokenFile(absolutePath);
	tokenFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&tokenFile);
	out<<encryptedToken;
	tokenFile.close();
}

QString SFOAuthCredentials::retreiveToken(QString key) const {
	QDir home = QDir::home();

	QString absoluteDirPath = home.absoluteFilePath(kSFTokenFileDir);
	QDir dir = QDir(absoluteDirPath);
	if (!dir.exists()){
		//return null
		return QString();
	}

	QString path = QString("%1/%2").arg(kSFTokenFileDir, key);
	QString absolutePath = home.absoluteFilePath(path);

	//qDebug()<<"token file path is "<<absolutePath;

	QFile tokenFile(absolutePath);
	tokenFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream out(&tokenFile);
	QString encryptedToken = out.readAll();
	tokenFile.close();

	//qDebug()<<"encrypted token string is "<<encryptedToken;

	QString tokenString = SFSecurityManager::instance()->decrypt(encryptedToken);

	//qDebug()<<"token string is "<<tokenString;

	return tokenString;
}


void SFOAuthCredentials::archiveValueToFile(QString key){
	this->archiveValueToFileInDir(key, kSFCredentialFileDir);
}
bool SFOAuthCredentials::unArchiveValueFromFile(QString key){
	return this->unArchiveValueFromFileInDir(key, kSFCredentialFileDir);
}

bool SFOAuthCredentials::removeCredentialArchive(QString key){
	return removeArchiveInDir(key, kSFCredentialFileDir);
}

QString SFOAuthCredentials::keyForToken(QString serviceType) const{
	return QString("%1-%2").arg(serviceType, mIdentifier);
}

}/*namespace*/
