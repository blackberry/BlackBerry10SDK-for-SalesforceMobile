/*
 * SFSecurityManager.cpp
 *
 *  Created on: Oct 22, 2013
 *      Author: timshi
 */

#include "SFSecurityManager.h"
#include <sbreturn.h>
#include "AESParams.hpp"
#include "AESKey.hpp"
#include "DRBG.hpp"
#include "SBError.hpp"
#include <QSettings>
#include "SHA.h"
#include "SFGlobal.h"

namespace sf {

static const QString kAESKey = "aes_key";
static const QString kAESIV = "aes_iv";

SFSecurityManager* SFSecurityManager::sharedInstance = NULL;

SFSecurityManager* SFSecurityManager::instance(){
	if (!sharedInstance){
		sharedInstance = new SFSecurityManager();
	}
	return sharedInstance;
}

QString SFSecurityManager::encrypt(QString clearText){
	QByteArray in(clearText.toUtf8());
	pad(in);
	QByteArray out(in.length(), 0);
	if (crypt(true, in, out)) {
		return toHex(out);
	}
	//return null string
	return QString();
}
QString SFSecurityManager::decrypt(QString cipherTextHex){
	QByteArray in;
	if (!fromHex(cipherTextHex, in)) {
		//return null string
		return QString();
	}
	QByteArray out(in.length(), 0);

	if (crypt(false, in, out)) {
		if (removePadding(out)) {
			return QString (QString::fromUtf8(out.constData(), out.length()));
		}
	}
	//return null string
	return QString();
}

QString SFSecurityManager::hash(QString clearText){
	unsigned char message_digest[SB_SHA256_DIGEST_LEN];
	SHA sha = SHA(mGlobalContext, message_digest);
	int rc = sha.updateDigest(clearText);
	if (!rc==SB_SUCCESS){
		return QString(); //returns null string
	}
	rc = sha.completeDigest();
	if (!rc==SB_SUCCESS){
		return QString(); //returns null string
	}
	QByteArray hash_data = QByteArray::fromRawData(reinterpret_cast<const char *>(message_digest), SB_SHA256_DIGEST_LEN);
	QString hash_as_hex = QString::fromAscii(hash_data.toHex());
	return hash_as_hex;
}


/*
 * private
 */
SFSecurityManager::SFSecurityManager() {
	QSettings setting;
	mKey = setting.value(kAESKey).toString();
	mIv = setting.value(kAESIV).toString();
	if (mKey.isNull() || mIv.isNull()){
		sfDebug()<<"generating new key and iv";
		mKey = generateRandomString();
		mIv = generateRandomString();
		setting.setValue(kAESKey, mKey);
		setting.setValue(kAESIV, mIv);
	}
}

SFSecurityManager::~SFSecurityManager() {
	//when this instance is destroyed, the global context will be destroyed and will clean up everything in the destructor
}

QString SFSecurityManager::generateRandomString(){
	DRBG drbg(mGlobalContext);
	QByteArray buffer(16, 0);
	int rc = drbg.getBytes(buffer);
	if (rc != SB_SUCCESS) {
		return "";
	}
	return toHex(buffer);
}

bool SFSecurityManager::fromHex(const QString in, QByteArray & toReturn) {
	QString temp(in);
	temp.replace(" ","");
	temp.replace(":","");
	temp.replace(".","");
	QByteArray content(temp.toLocal8Bit());

	const char * c(content.constData());

	if (content.length() == 0 || ((content.length() % 2) != 0)) {
		return false;
	}

	for (int i = 0; i < content.length(); i += 2) {
		char a = c[i];
		char b = c[i + 1];
		a = nibble(a);
		b = nibble(b);
		if (a < 0 || b < 0) {
			toReturn.clear();
			return false;
		}
		toReturn.append((a << 4) | b);
	}
	return true;
}

QString SFSecurityManager::toHex(const QByteArray & in){
	static char hexChars[] = "0123456789abcdef";

	const char * c = in.constData();
	QString toReturn;
	for (int i = 0; i < in.length(); ++i) {
		toReturn += hexChars[(c[i] >> 4) & 0xf];
		toReturn += hexChars[(c[i]) & 0xf];
	}

	return toReturn;
}

char SFSecurityManager::nibble(char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	} else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	} else if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	return -1;
}

void SFSecurityManager::pad(QByteArray & in){
	int padLength = 16 - (in.length() % 16);
	for (int i = 0; i < padLength; ++i) {
		in.append((char) padLength);
	}
}

bool SFSecurityManager::removePadding(QByteArray & out) {
	char paddingLength = out[out.length() - 1];
	if (paddingLength < 1 || paddingLength > 16) {
		out.clear();
		return false;
	}
	if (paddingLength > out.length()) {
		out.clear();
		return false;
	}
	for (int i = 1; i < paddingLength; ++i) {
		char next = out[out.length() - 1 - i];
		if (next != paddingLength) {
			out.clear();
			return false;
		}
	}
	out.remove(out.length() - paddingLength, paddingLength);
	return true;
}

bool SFSecurityManager::crypt(bool isEncrypt, const QByteArray & in, QByteArray & out){
	QByteArray key, iv;
	QString fail;

	if (!fromHex(mKey, key)) {
		fail += "Key is not valid hex. ";
	}
	if (!fromHex(mIv, iv)) {
		fail += "IV is not valid hex. ";
	}

	if (!fail.isEmpty()) {
		return false;
	}

	AESParams params(mGlobalContext);
	if (!params.isValid()) {
		return false;
	}

	AESKey aesKey(params, key);
	if (!aesKey.isValid()) {
		return false;
	}

	int rc;
	if (isEncrypt) {
		rc = hu_AESEncryptMsg(params.aesParams(), aesKey.aesKey(), iv.length(),
				(const unsigned char*) iv.constData(), in.length(),
				(const unsigned char *) in.constData(),
				(unsigned char *) out.data(), mGlobalContext.ctx());
	} else {
		rc = hu_AESDecryptMsg(params.aesParams(), aesKey.aesKey(), iv.length(),
				(const unsigned char*) iv.constData(), in.length(),
				(const unsigned char *) in.constData(),
				(unsigned char *) out.data(), mGlobalContext.ctx());
	}
	if (rc == SB_SUCCESS) {
		return true;
	}
	return false;
}
} /* namespace sf */
