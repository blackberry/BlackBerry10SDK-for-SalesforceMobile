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
* SFResult.h
*
*  Created on: Mar 6, 2013
*      Author: Livan Yi Du
*/

#ifndef SFRESULT_H_
#define SFRESULT_H_

#include <QObject>
#include <QVariant>
#include <QWeakPointer>
#include "SFGlobal.h"

class QScriptValue;
class QScriptEngine;

namespace sf {
class SFRestResourceTask;
class SFGenericTask;

/*!
 * @class SFResult
 * @headerfile SFResult.h <core/SFResult.h>
 * @brief Generic wrapper class that carry result of any task performed by the SDK.
 *
 * @details This class is designed to carry information regarding to the result of an operation/task. The class also implemented
 * a payload smart conversion system for convenience.
 *
 * @see SFResultCode, SFGenericTask, SFNetworkAccessTask, SFRestResourceTask
 *
 * \author Livan Yi Du
 */
class SFResult: public QObject {
	Q_OBJECT
	Q_ENUMS(TaskResult)
	Q_PROPERTY(TaskResult status READ status) /*!< The status code of the corresponding task. See @c TaskResult */
	Q_PROPERTY(bool hasError READ hasError) /*!< Indicating whether the task finished with error */
	Q_PROPERTY(int code READ code) /*!< The code of the result. Common code values are defined in @c SFResultCode @note This property serves the purpose of categorizing the result. A non-zero code doesn't imply error. */
	Q_PROPERTY(QString message READ message) /*!< A human friendly message string for @a code, localized or not localized. */
	Q_PROPERTY(QVariant payload READ payload) /*!< The payload/content of the result. May contains any kind of data. Use @c SFResult::payload<T>() if you want auto-convert it */
	Q_PROPERTY(QVariantHash tags READ tags) /*!< A hash map that holds additional objects */
	friend class SFGenericTask;
	friend class SFRestResourceTask;
	friend class SFNetworkAccessTask;
public:
	/*! Task's result code */
	enum TaskResult {
		TaskResultSuccess= 0, /*!< Task finished successfully */
		TaskResultError = 1, /*!< Task finished with error */
		TaskResultCancelled = 2, /*!< Task is canceled. */
		TaskResultNotAvailable= -1, /*!< Result is not determined. Default value for new tasks. */
	};

	SFResult();  /*!< Default constructor */
	SFResult(QObject *parent);  /*!< Constructor with parent QObject */
	virtual ~SFResult();  /*!< Default destructor */

	/*! Convenient function to create an empty result object.
	 * @return pointer to the created object */
	static SFResult *create();
	/*! Convenient function to create a result object with given code and message
	 * @param code the code of the result. Common code values are defined in @c SFResultCode
	 * @param message locolized/non-locolized human readable message string.
	 * @return pointer to the created object */
	static SFResult *createErrorResult(const int & code, const QString & message);
	/*! Convenient function to create a result object with "canceled" status
	 * @param code the code of the result. Common code values are defined in @c SFResultCode. Default: SFResultCode::SFErrorCancelled
	 * @param message locolized/non-locolized human readable message string.
	 * @return pointer to the created object */
	static SFResult *createCancelResult(const int & code = SFResultCode::SFErrorCancelled, const QString & message = "Task Cancelled");

	/*! @see SFResult::status @return the status code */
	TaskResult status() {return mStatus;};
	/*! @see SFResult::code @return the code of the result */
	const int & code() {return mCode;};
	/*! @see SFResult::message @return the message */
	const QString & message() {return mMessage;};
	/*! @see SFResult::hasError @return true if the result is an error result. Otherwise, false */
	bool hasError() {return mStatus != TaskResultSuccess && mStatus != TaskResultNotAvailable;};
	/*! Get the raw payload of the result as @c QVariant. If auto-conversion is needed, use the template version @c SFResult::payload<T>()
	 * @see SFResult::payload
	 * @return the raw payload of the result as @c QVariant. */
	const QVariant & payload() {return mPayload;};
	/*! Set the raw payload
	 * @see SFResult::payload
	 * @param payload the payload encapsulated in QVariant */
	void setPayload(const QVariant & payload) { mPayload = payload;};

	/*! @see SFResult::tags @return the hash map of all additional objects. */
	const QVariantHash & tags() { return mTags;};

	/*! Add an additional object and associate it with a key
	 * @param key the key associated with the object
	 * @param tag the object */
	void putTag(const QString & key, const QVariant & tag) {mTags[key] = tag;};

	/*! Get the payload as specified type T
	 * @return an object with the specified type T. @note When the payload cannot be converted to the specified type: <br>return NULL if T is a pointer
	 * type, <br>return an object using default constructor if T is a class type. */
	template<class T>
	T payload() {
		return QVariantConverter<T>::convert(mPayload);
	};

	/*! Get the additional object associated with given key
	 * @param key the key
	 * @return the object associated with the @a key */
	template<class T>
	T getTag(const QString & key) { return QVariantConverter<T>::convert(mTags[key]);};

	/* QScriptEngine conversin functions */
	/* In order to completely use this QObject type in QML javascript, we need to provide conversion functions
	 * and Q_DECLARE_METATYPE(). This is necessary for javascript functions to recieve this type as parameter
	 */
	/*! Serializing function that can be registered with QScriptEngine */
	static QScriptValue toScriptValue(QScriptEngine *engine, SFResult* const &inObj);
	/*! Deserializing function that can be registered with QScriptEngine */
	static void fromScriptValue(const QScriptValue & obj, SFResult * &outObj);

private:
	TaskResult mStatus;
	int mCode;
	QString mMessage;
	QVariantHash mTags;
	QVariant mPayload;

	/*conversion: common Qt data types*/
	template<class T>
	class QVariantConverter {
	public:
		static inline T convert(const QVariant & payload) {
			return payload.value<T>();
		}
	};
	/*conversion: pointer types*/
	template<class T>
	class QVariantConverter<T*> {
	public:
		static inline T* convert(const QVariant & payload) {
			return static_cast<T*>(doConvert(payload, static_cast<T*>(0)));
		};
		/*conversion: QObject pointers*/
		static inline QObject * doConvert(const QVariant & payload, QObject *) {
			QObject *ptr = payload.value<QObject*>();
			if (!ptr) {
				ptr = payload.value<QWeakPointer<QObject> >().data();
			}
			return ptr;
		};
		/*conversion: other pointers*/
		static inline void * doConvert(const QVariant & payload, void *) {return payload.value<void*>();};
	};

private:
};

} /* namespace sf */

Q_DECLARE_METATYPE(QWeakPointer<QObject>)
Q_DECLARE_METATYPE(QList<QVariantList>)
Q_DECLARE_METATYPE(sf::SFResult*)
#endif /* SFRESULT_H_ */
