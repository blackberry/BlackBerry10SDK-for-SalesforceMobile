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
* SFResult.cpp
*
*  Created on: Mar 6, 2013
*      Author: Livan Yi Du
*/

#include "SFResult.h"
#include <QtScript/QtScript>

namespace sf {

SFResult::SFResult() {
	mStatus = TaskResultNotAvailable;
	mCode = 0;
	mMessage = QString();
	mPayload = QVariant();

}
SFResult::SFResult(QObject* parent) : QObject(parent) {
	mStatus = TaskResultNotAvailable;
	mCode = 0;
	mMessage = QString();
	mPayload = QVariant();
	if (parent) {
		setObjectName(parent->objectName());
	}

}

SFResult::~SFResult() {

}

SFResult * SFResult::create() {
	SFResult *result = new SFResult(0); //no parent
	result->mStatus = TaskResultSuccess;
	return result;
}

SFResult * SFResult::createErrorResult(const int & code, const QString & message) {
	SFResult *result = new SFResult(0); //no parent
	result->mStatus = SFResult::TaskResultError;
	result->mCode = code;
	result->mMessage = message;
	return result;
}

SFResult * SFResult::createCancelResult(const int & code, const QString & message) {
	SFResult *result = new SFResult(0); //no parent
	result->mStatus = SFResult::TaskResultCancelled;
	result->mCode = code;
	result->mMessage = message;
	return result;
}

/* Conversion function for QScriptEngine */
QScriptValue SFResult::toScriptValue(QScriptEngine *engine, SFResult* const &inResult) {
  return engine->newQObject(inResult, QScriptEngine::QtOwnership);
}

void SFResult::fromScriptValue(const QScriptValue & obj, SFResult * &outResult) {
	outResult = qobject_cast<SFResult*>(obj.toQObject());
}

} /* namespace sf */



