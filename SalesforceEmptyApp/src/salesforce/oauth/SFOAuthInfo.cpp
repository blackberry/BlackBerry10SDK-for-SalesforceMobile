/*
 * SFOAuthInfo.cpp
 *
 *  Created on: Oct 13, 2013
 *      Author: timshi
 */

#include "SFOAuthInfo.h"

namespace sf {

SFOAuthInfo::SFOAuthInfo() {
	mAuthType=SFOAuthTypeUnknown;
}

SFOAuthInfo::~SFOAuthInfo() {
}

SFOAuthType SFOAuthInfo::getAuthType() const {
	return mAuthType;
}

void SFOAuthInfo::setAuthType(SFOAuthType authType) {
	this->mAuthType = authType;
}
}
