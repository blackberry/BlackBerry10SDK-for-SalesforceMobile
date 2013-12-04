/*
 * SFPluginResult.h
 *
 *  Created on: Nov 27, 2013
 *      Author: timshi
 */

#ifndef SFPLUGINRESULT_H_
#define SFPLUGINRESULT_H_

#include <QVariant>

namespace sf {

typedef enum {
    SFCommandStatus_NO_RESULT = 0,
    SFCommandStatus_OK,
    SFCommandStatus_CLASS_NOT_FOUND_EXCEPTION,
    SFCommandStatus_ILLEGAL_ACCESS_EXCEPTION,
    SFCommandStatus_INSTANTIATION_EXCEPTION,
    SFCommandStatus_MALFORMED_URL_EXCEPTION,
    SFCommandStatus_IO_EXCEPTION,
    SFCommandStatus_INVALID_ACTION,
    SFCommandStatus_JSON_EXCEPTION,
    SFCommandStatus_ERROR
} SFPluginCommandStatus;

/*
 * The javascript callback is responsible for checking status and handle message accordingly
 */
class SFPluginResult {
public:
	SFPluginResult(SFPluginCommandStatus status, const QVariant& message, bool keepCallback);
	virtual ~SFPluginResult();
	bool isKeepCallback() const {return mKeepCallback;}
	const QVariant& getMessage() const {return mMessage;}
	SFPluginCommandStatus getStatus() const {return mStatus;}

private:
	SFPluginResult();
	SFPluginCommandStatus mStatus;
	QVariant mMessage;
	bool mKeepCallback;
};

} /* namespace sf */
#endif /* SFPLUGINRESULT_H_ */
