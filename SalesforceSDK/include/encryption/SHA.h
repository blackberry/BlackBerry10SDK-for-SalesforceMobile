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
