/*
 * SFNetworkAccessTask.h
 *
 *  Created on: Apr 3, 2013
 *      Author: Livan Yi Du
 */

#ifndef SFNETWORKACCESSTASK_H_
#define SFNETWORKACCESSTASK_H_

#include "SFGenericTask.h"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include "SFGlobal.h"

class QTimer;

namespace sf {

class SFNetworkAccessTask : public sf::SFGenericTask {
	Q_OBJECT
public slots:
	void onReplyFinished();
	void onNetworkTimeout();
signals:
	void taskWillRetry();
	void taskDidRetry();

public:
	SFNetworkAccessTask(QNetworkAccessManager * const networkAccessManager);
	SFNetworkAccessTask(QNetworkAccessManager * const networkAccessManager, const QNetworkRequest & request, const HTTPMethodType & method = HTTPMethod::HTTPGet);
	SFNetworkAccessTask(QNetworkAccessManager * const networkAccessManager, const QUrl & url, const HTTPMethodType & method = HTTPMethod::HTTPGet);
	SFNetworkAccessTask(QNetworkAccessManager * const networkAccessManager, const QString & path, const HTTPMethodType & method = HTTPMethod::HTTPGet);
	virtual ~SFNetworkAccessTask();

	Q_INVOKABLE virtual void startTaskAsync(QObject* resultReceiver = NULL, const char * resultReceiverSlot = NULL);

	/* Accessors */
	QNetworkRequest & request() { return this->mRequest;};

	bool isUseCache() { return this->mUseCache;};
	void setUseCache(bool useCache) { this->mUseCache = useCache;};

	unsigned long int networkTimeout() {return this->mNetworkTimeout;};
	void setNetworkTimeout(unsigned long int msec) { this->mNetworkTimeout = msec;};

	void setRequestAttribute(QNetworkRequest::Attribute code, const QVariant & value) {mRequest.setAttribute(code, value);};
	QVariant requestAttribute(QNetworkRequest::Attribute code) {return mRequest.attribute(code);};

	QByteArray requestBytesArray() { return this->mRequestBytesArray;};
	/* passing non-null bytes array will clear any QIODevice assigned earlier via "setRequestData" */
	void setRequestBytesArray(const QByteArray & bytes);

	QIODevice *requestData() { return this->mRequestData;};
	/* this function takes ownership. Passing non-null QIODevice will clear any QByteArray assigned earlier via "setRequestBytesArray" */
	void setRequestData(QIODevice * data);

protected:
	enum NetworkTaskState {
		StateNotStarted,
		StateReadyToSend,
		StateWaiting,
		StateHasResponse,
		StateNeedToRedirect,
		StateReadyToProcess,
		StateProcessing,
		StateFinished,
		StateError,
		StateNeedToRetry,
	};

	QNetworkAccessManager *mNetworkAccessManager;
	QNetworkRequest mRequest;
	HTTPMethodType mMethod;
	QByteArray mRequestBytesArray;
	QIODevice *mRequestData;
	NetworkTaskState mState;
	QNetworkReply *mCurrentReply;
	bool mUseCache;
	unsigned long int mNetworkTimeout;
	QTimer *mNetworkTimer;

	virtual void run();
	virtual void prepare();
	virtual TaskStatus execute() {throw std::exception(); };
	virtual void cleanup();
	virtual bool retry();

	Q_INVOKABLE void moveQObjectsToThread(QThread *thread);
	Q_INVOKABLE void fsmDispatcher();
	NetworkTaskState initiateNetworkAccess();
	QNetworkReply * createReplyAndExit();

	virtual NetworkTaskState ensureRequest();
	virtual NetworkTaskState preprocessReply(QNetworkReply * reply);
	virtual NetworkTaskState processReply(QNetworkReply * reply);

	//debug
	QString composeReplyHeader(QNetworkReply * reply);
	QString stateDescription(NetworkTaskState state);

};
//typedef sf::SFNetworkAccessTask::HTTPMethod HTTPMethod;

} /* namespace */
#endif /* SFNETWORKACCESSTASK_H_ */
