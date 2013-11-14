/*
 * SHA.cpp
 *
 *  Created on: Nov 4, 2013
 *      Author: timshi
 */

#include "SHA.h"
#include "husha2.h"
#include "sbreturn.h"

namespace sf {

SHA::SHA(GlobalContext & gc, unsigned char* messageDigest):
		Crypto("SHA"), context(gc), sha256Context(NULL) {
	mMessageDigest = messageDigest;
	int rc = hu_SHA256Begin((size_t) SB_SHA256_DIGEST_LEN, NULL, &sha256Context, context.ctx());
	maybeLog("SHA begin", rc);
}

int SHA::updateDigest(const QString input_data){
	QByteArray input_bytes = input_data.toUtf8();
	unsigned char* hash_input = reinterpret_cast<unsigned char*>(input_bytes.data());

	int rc = hu_SHA256Hash(sha256Context, (size_t) input_bytes.length(), hash_input, context.ctx());
	maybeLog("SHA creating hash", rc);
	if (rc!=SB_SUCCESS){
		return rc;
	}
	return rc;
}

int SHA::completeDigest(){
	int rc = hu_SHA256End(&sha256Context, mMessageDigest, context.ctx());
	maybeLog("SHA completing hash", rc);
	if (rc!=SB_SUCCESS){
		return rc;
	}
	return rc;
}

SHA::~SHA() {
	if (sha256Context!=NULL){
		int rc = hu_SHA256End(&sha256Context, NULL, context.ctx());
		maybeLog("SHA cleanup", rc);
		sha256Context=NULL;
	}
}
} /* namespace sf */
