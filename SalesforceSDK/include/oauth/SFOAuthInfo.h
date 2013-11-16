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
* SFOAuthInfo.h
*
*  Created on: Oct 13, 2013
*      Author: timshi
*/

#ifndef SFOAUTHINFO_H_
#define SFOAUTHINFO_H_

#include <QObject>

namespace sf {

typedef enum {
    SFOAuthTypeUnknown = 0,
    SFOAuthTypeUserAgent,
    SFOAuthTypeRefresh
} SFOAuthType;

/*!
 * @class SFOAuthInfo
 * @headerfile SFOAuthInfo.h <oauth/SFOAuthInfo.h>
 *
 * @brief  Data class containing members denoting state information for an OAuth coordinator authentication cycle.
 *
 */
class SFOAuthInfo : public QObject {
	Q_OBJECT
public:
	SFOAuthInfo();
	virtual ~SFOAuthInfo();

	SFOAuthType getAuthType() const;
	void setAuthType(SFOAuthType authType);

private:
	SFOAuthType mAuthType;
};

}
#endif /* SFOAUTHINFO_H_ */
