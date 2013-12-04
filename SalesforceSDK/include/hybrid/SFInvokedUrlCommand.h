/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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
