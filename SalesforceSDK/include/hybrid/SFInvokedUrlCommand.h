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

/*!
 * @class SFInvokedUrlCommand
 * @headerfile SFInvokedUrlCommand <hybrid/SFInvokedUrlCommand.h>
 * @brief The SFInvokedUrlCommand encapsulates the information that javascript needs to pass to native code in order to trigger a method on a plugin
 *
 * @details
 * When the post a message to native code, the message is deserialized into SFInvokedUrlCommand object. Based on the information stored in this object,
 * the native code can find the appropriate plugin instance and executed the desired method on that instance.
 *
 *\sa SFHybridApplicationui
 */

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
	/*!
	 * @return arguments The arguments is deserialized from json when they are passed in from javascript
	 */
	const QMap<QString, QVariant>& getArguments() const {return mArguments;}
	/*!
	 * @param arguments The deserialized json arguments.
	 */
	void setArguments(const QMap<QString, QVariant>& arguments) {mArguments = arguments;}
	/*!
	 * @return callbackId that identifies the callback function in javascript
	 */
	const QString& getCallbackId() const {return mCallbackId;}
	/*!
	 * @param callbackId The callbackId that identifies the callback function in javascript
	 */
	void setCallbackId(const QString& callbackId) {mCallbackId = callbackId;}
	/*!
	 * @return fully qualified class name of the plugin to call
	 */
	const QString& getClassName() const {return mClassName;}
	/*!
	 * @param className fully qualified class name of the plugin to call
	 */
	void setClassName(const QString& className) {mClassName = className;}
	/*!
	 * @return name of the method to be called on the plugin
	 */
	const QString& getMethodName() const {return mMethodName;}
	/*!
	 * @param methodName name of the method to be called on the plugin
	 */
	void setMethodName(const QString& methodName) {mMethodName = methodName;}
};

} /* namespace sf */
#endif /* SFINVOKEDURLCOMMAND_H_ */
