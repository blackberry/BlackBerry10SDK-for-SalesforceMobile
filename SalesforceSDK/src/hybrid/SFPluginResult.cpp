/*
 * SFPluginResult.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: timshi
 */

#include "SFPluginResult.h"

namespace sf {

SFPluginResult::SFPluginResult(){
	mStatus = SFCommandStatus_NO_RESULT;
	mKeepCallback = false;
}


SFPluginResult::SFPluginResult(SFPluginCommandStatus status, const QVariant& message, bool keepCallback)
{
	mStatus = status;
	mMessage = message;
	mKeepCallback = keepCallback;
}

SFPluginResult::~SFPluginResult() {
}

} /* namespace sf */
