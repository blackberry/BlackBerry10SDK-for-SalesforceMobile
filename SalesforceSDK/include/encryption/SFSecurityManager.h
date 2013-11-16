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
/*!
 * @class SFSecurityManager
 * @headerfile SFSecurityManager.h <encryption/SFSecurityManager.h>
 *
 * @brief A singleton class that will provides functions for AES encryption and SHA.
 */
class SFSecurityManager : public QObject {
	Q_OBJECT

private:
	static SFSecurityManager* sharedInstance;
	QString mKey,mIv;
    GlobalContext mGlobalContext;

public:
    /*!
     * @return the singleton instance
     */
	static SFSecurityManager* instance();
	/*!
	 * @param clearText to be encrypted
	 * @return the encrypted string
	 */
	QString encrypt(QString clearText);
	/*!
	 * @param cipherTextHex the encoded string in hex format
	 * @return decoded string
	 */
	QString decrypt(QString cipherTextHex);
	/*!
	 * @param clearText to be hashed
	 * @return the hashed string
	 */
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
