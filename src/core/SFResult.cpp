/*
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



