/*
 * SHA.h
 *
 *  Created on: Nov 4, 2013
 *      Author: timshi
 */

#ifndef SHA_H_
#define SHA_H_

#include "GlobalContext.hpp"

#define SB_SHA256_DIGEST_LEN 32

namespace sf {

class SHA : public Crypto{
public:
	SHA(GlobalContext & context, unsigned char* messageDigest);
	virtual ~SHA();
	virtual bool isValid() {
		return sha256Context!=NULL;
	}
	int updateDigest(const QString input_data);
	int completeDigest();
private:
	GlobalContext & context;
	sb_Context sha256Context;
	unsigned char* mMessageDigest;
};

} /* namespace sf */
#endif /* SHA_H_ */
