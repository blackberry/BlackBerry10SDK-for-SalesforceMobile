/*
 * SFInvokedUrlCommand.h
 *
 *  Created on: Nov 27, 2013
 *      Author: timshi
 */

#ifndef SFINVOKEDURLCOMMAND_H_
#define SFINVOKEDURLCOMMAND_H_

#include <QString>
#include <QMap>

namespace sf {

class SFInvokedUrlCommand {
public:
	SFInvokedUrlCommand();
	virtual ~SFInvokedUrlCommand();

private:
	QString mCallbackId;
	QString mClassName;
	QString mMethodName;
	QMap<QString, QVariant> mArguments;

public:
	const QMap<QString, QVariant>& getArguments() const {return mArguments;}
	void setArguments(const QMap<QString, QVariant>& arguments) {mArguments = arguments;}

	const QString& getCallbackId() const {return mCallbackId;}
	void setCallbackId(const QString& callbackId) {mCallbackId = callbackId;}

	const QString& getClassName() const {return mClassName;}
	void setClassName(const QString& className) {mClassName = className;}

	const QString& getMethodName() const {return mMethodName;}
	void setMethodName(const QString& methodName) {mMethodName = methodName;}
};

} /* namespace sf */
#endif /* SFINVOKEDURLCOMMAND_H_ */
