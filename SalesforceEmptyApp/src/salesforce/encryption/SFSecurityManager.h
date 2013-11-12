/*
 * SFSecurityManager.h
 *
 *  Created on: Oct 22, 2013
 *      Author: timshi
 */

#ifndef SFSECURITYMANAGER_H_
#define SFSECURITYMANAGER_H_

#include <QObject>
#include <qbytearray.h>
#include "GlobalContext.hpp"

namespace sf {

/*
 * singleton that will manage the encryption context
 * should be connected to the application's aboutToQuite signal so it can clean up when the app quits
 *
 * uses AES encryption, saves the key and iv in QSetting
 */

class SFSecurityManager : public QObject {
	Q_OBJECT

private:
	static SFSecurityManager* sharedInstance;
	QString mKey,mIv;
    GlobalContext mGlobalContext;

public:
	static SFSecurityManager* instance();
	//encrypts the clear text string in hex format
	QString encrypt(QString clearText);
	//expects an encoded string in hex format and decodes it to clear text
	QString decrypt(QString cipherTextHex);
	//returns the hashed value of the clear text string
	QString hash(QString clearText);

private:
	SFSecurityManager();
	virtual ~SFSecurityManager();
	//generate random string (for use as key and iv)
	QString generateRandomString();
	//hex conversion
	bool fromHex(const QString in, QByteArray & toReturn);
	QString toHex(const QByteArray & in);
    char nibble(char);
    //padding
    void pad(QByteArray & in);
    bool removePadding(QByteArray & out);
    //internal function used to encrypt and decrypt
    bool crypt(bool isEncrypt, const QByteArray & in, QByteArray & out);
};

} /* namespace sf */
#endif /* SFSECURITYMANAGER_H_ */
