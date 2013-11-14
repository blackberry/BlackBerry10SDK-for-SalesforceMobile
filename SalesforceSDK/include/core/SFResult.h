/*
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
class SFResult: public QObject {
	Q_OBJECT
	Q_ENUMS(TaskResult);
	Q_ENUMS(SFErrorCode);
	Q_PROPERTY(TaskResult status READ status);
	Q_PROPERTY(bool hasError READ hasError);
	Q_PROPERTY(int code READ code);
	Q_PROPERTY(QString message READ message);
	Q_PROPERTY(QVariant payload READ payload);
	Q_PROPERTY(QVariantHash tags READ tags);
	friend class SFGenericTask;
	friend class SFRestResourceTask;
	friend class SFNetworkAccessTask;
public:
	enum TaskResult {
		TaskResultSuccess= 0,
		TaskResultError = 1,
		TaskResultCancelled = 2,
		TaskResultNotAvailable= -1,
	};

	SFResult();
	SFResult(QObject *parent);
	virtual ~SFResult();
	static SFResult *create();
	static SFResult *createErrorResult(const int & code, const QString & message);
	static SFResult *createCancelResult(const int & code = SFResultCode::SFErrorCancelled, const QString & message = "Task Cancelled");
	TaskResult status() {return mStatus;};
	const int & code() {return mCode;};
	const QString & message() {return mMessage;};
	bool hasError() {return mStatus != TaskResultSuccess && mStatus != TaskResultNotAvailable;};
	const QVariant & payload() {return mPayload;};
	const QVariantHash & tags() { return mTags;};
	void putTag(const QString & key, const QVariant & tag) {mTags[key] = tag;};

	template<class T>
	T payload() {
		return QVariantConverter<T>::convert(mPayload);
	};

	template<class T>
	T getTag(const QString & key) { return QVariantConverter<T>::convert(mTags[key]);};

	/* QScriptEngine conversin functions */
	/* In order to completely use this QObject type in QML javascript, we need to provide conversion functions
	 * and Q_DECLARE_METATYPE(). This is necessary for javascript functions to recieve this type as parameter
	 */
	static QScriptValue toScriptValue(QScriptEngine *engine, SFResult* const &inObj);
	static void fromScriptValue(const QScriptValue & obj, SFResult * &outObj);

protected:
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
