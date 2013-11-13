/*
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
