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

/*!
 * @class SFNetworkAccessTask
 * @headerfile SFNetworkAccessTask.h <core/SFNetworkAccessTask.h>
 * @brief A class for performing basic HTTP/HTTPS transactions.
 *
 * @details This class is a concrete implementation of abstract class @c SFGenericTask. It's designed to send HTTP request and receive response
 * asynchronously and concurrently. The purpose of this class is to provide a base implementation for any complex network transactions.
 * The class is implemented using Finite-State-Machine(FSM) approach and is capable of running any response processing
 * in a separated thread from QThreadPool.
 *
 * Usage
 * ------
 * For most REST requests with Force.com, developer should use @c SFRestResourceTask which is a subclass of this. However, this class
 * can be used directly for any non-RESTful transactions:
 * @code
 * //Prepare HTTP request
 * QNetworkRequest request("https://www.webservice.com/path/to/the/service");
 * QByteArray body;
 *
 * //Configure your request...
 *
 * //Prepare the task
 * SFNetworkAccessTask *task = new SFNetworkAccessTask(getSharedNetworkAccessManager(), request, HTTPMethod::HTTPPost);
 * task->setRequestBytesArray(body.toUtf8());
 *
 * //Enable cancel
 * task->setCancellable(true);
 * connect(pCancelButton, SIGNAL(clicked()), task, SLOT(cancel()));
 *
 * //Connect slots, schedule and run
 * task->startTaskAsync(this, SLOT(onNetworkReplyReady(sf::SFResult*)));
 * @endcode
 * And in your slot:
 * @code
 * void SFOAuthCoordinator::onNetworkReplyReady(sf::SFResult* result) {
 * 	if (result->hasError()) {
 * 		//Handle error
 * 	} else {
 * 		QNetworkReply *reply = result->payload<QNetworkReply*>();
 * 		if (reply) {
 * 			//Handle the reply
 * 		}
 * 	}
 * }
 * @endcode
 *
 * @anchor SFNetworkAccessTask_Subclass
 * Subclassing Notes
 * ------------------
 *
 * Typically, for any Force.com REST API, you should use @c SFRestResourceTask. In addition, the source code of @c SFRestResourceTask
 * provides a good example of subclassing @c SFNetworkAccessTask. You can add your custom logic by overriding following
 * functions:
 * - @c prepare() called as soon as the task is started. The function is executed in main thread (or the thread this task belongs to)
 *
 * - @c cleanup() called when the task is finished and ready to deliver the result. The base class implementation does the memory cleanup
 * and emits signals in proper order. This function also attempt to re-try if enabled. So please call base class implementation at the end of your custom logic.
 * This function can be executed in any thread.
 *
 * - @c retry() called when auto re-try is triggered. This function reset the task and cleanup memory allocated during the operation.
 * This function is invoked from @c cleanup(), so it can be executed in any thread.
 *
 * - @c ensureRequest() called before the request is sent. It is executed only in the task's owning thread. Please see @c ensureRequest()
 * for possible returned states.
 *
 * - @c preprocessReply(QNetworkReply*) called when each @c QNetworkReply finished. It is executed only in the task's owning thread.
 * The main responsibility of this function is to handles HTTP redirect or caching. Please see @c preprocessReply(QNetworkReply*)
 * for possible returned states.
 *
 * - @c processReply(QNetworkReply*) called when the reply is ready to be processed. The function is always executed in a separate thread.
 * Please see @c processReply(QNetworkReply*) for possible returned states.
 *
 * @see SFResult, SFGenericTask, SFRestResourceTask
 *
 * \author Livan Yi Du
 */
class SFNetworkAccessTask : public sf::SFGenericTask {
	Q_OBJECT
private slots:
	void onReplyFinished();
	void onNetworkTimeout();
signals:
	void taskWillRetry(); /*!< Emitted before the task re-start itself. */
	void taskDidRetry(); /*!< Emitted after the task re-start itself. */

public:
	/*! @param networkAccessManager the shared QNetworkAccessManager instance */
	SFNetworkAccessTask(QNetworkAccessManager * const networkAccessManager);
	/*! @param networkAccessManager the shared QNetworkAccessManager instance
	 * @param request a instance of QNetworkRequest
	 * @param method the HTTP verb */
	SFNetworkAccessTask(QNetworkAccessManager * const networkAccessManager, const QNetworkRequest & request, const HTTPMethodType & method = HTTPMethod::HTTPGet);
	/*! @param networkAccessManager the shared QNetworkAccessManager instance
	 * @param url the absolute URL of your network task. HTTP or HTTPS
	 * @param method the HTTP verb */
	SFNetworkAccessTask(QNetworkAccessManager * const networkAccessManager, const QUrl & url, const HTTPMethodType & method = HTTPMethod::HTTPGet);
	/*! @param networkAccessManager the shared QNetworkAccessManager instance
	 * @param path the absolute path of the URL. Should start with HTTP or HTTPS
	 * @param method the HTTP verb */
	SFNetworkAccessTask(QNetworkAccessManager * const networkAccessManager, const QString & path, const HTTPMethodType & method = HTTPMethod::HTTPGet);
	/*! Default destructor */
	virtual ~SFNetworkAccessTask();

	/*! Start the task asynchronously.
	 * @param resultReceiver The object that handles the result
	 * @param resultReceiverSlot The slot description string. Should be a string generated using macro SLOT() */
	Q_INVOKABLE void startTaskAsync(QObject* resultReceiver = NULL, const char * resultReceiverSlot = NULL);

	/* Accessors */
	/*! @return the @c QNetworkRequest instance */
	QNetworkRequest & request() { return this->mRequest;};

	/*! @return whether the network task should try to use the cache installed in @c QNetworkAccessManager. @see setUserCache() */
	bool isUseCache() { return this->mUseCache;};
	/*! Set whether the network task should try to use cache. If cache is not configured in given @c QNetworkAccessManager instance,
	 * setting this flag takes no effect.
	 * @param useCache whether use cache */
	void setUseCache(bool useCache) { this->mUseCache = useCache;};

	/*! @return the amount of time in milliseconds that the task should wait for, before canceling the network transaction */
	unsigned long int networkTimeout() {return this->mNetworkTimeout;};
	/*! Set the amount of time in milliseconds that the task should wait for, before canceling the network transaction */
	void setNetworkTimeout(unsigned long int msec) { this->mNetworkTimeout = msec;};

	/*! Set additional attributes to the network request
	 * @param code the code of the attribute.
	 * @param value the value of the attribute. */
	void setRequestAttribute(QNetworkRequest::Attribute code, const QVariant & value) {mRequest.setAttribute(code, value);};
	/*! @return the attribute associated with given attribute code */
	QVariant requestAttribute(QNetworkRequest::Attribute code) {return mRequest.attribute(code);};

	/*! @return the bytes array that will be sent as request's content */
	QByteArray requestBytesArray() { return this->mRequestBytesArray;};
	/*! Set the bytes array that will be sent as request's content. Passing non-null bytes array will clear any
	 * QIODevice assigned earlier via @c setRequestData() */
	void setRequestBytesArray(const QByteArray & bytes);

	/*! @return the @c QIODevice that contains the request's content data */
	QIODevice *requestData() { return this->mRequestData;};
	/*! Set a @c QIODevice that contains data to sent as request's content. This function takes ownership. Passing non-null
	 * @c QIODevice will clear any @c QByteArray assigned earlier via @c setRequestBytesArray() */
	void setRequestData(QIODevice * data);

protected:
	/*! Possible states of the task */
	enum NetworkTaskState {
		StateNotStarted, /*!< The task is not started yet. */
		StateReadyToSend, /*!< The request of this task is ready to be sent. */
		StateWaiting, /*!< The task is waiting for network response. */
		StateHasResponse, /*!< The task has received a response from server. */
		StateNeedToRedirect, /*!< The task need to re-sent a request with different URL. */
		StateReadyToProcess, /*!< The task has a response that is ready to be processed. */
		StateProcessing, /*!< The task is processing the response (in a separate thread). */
		StateFinished, /*!< The task is finished without error. */
		StateError, /*!< The task is finished with error, re-try is not an option. */
		StateNeedToRetry, /*!< The task is finished with error. It will re-try later. */
	};

	QNetworkAccessManager *mNetworkAccessManager; /*!< Holds a shared instance of @c QNetworkAccessManager */
	QNetworkRequest mRequest; /*!< Holds a instance of @c QNetworkRequest */
	HTTPMethodType mMethod; /*!< Holds the HTTP verb of the request */
	QByteArray mRequestBytesArray; /*!< Holds the data of request's content */
	QIODevice *mRequestData; /*!< Holds a @c QIODevice which contains the data of request's content */
	NetworkTaskState mState; /*!< Current state of the task */
	QNetworkReply *mCurrentReply; /*!< Current @c QNetworkReply instance */
	bool mUseCache; /*!< Whether should use cache if possible */
	unsigned long int mNetworkTimeout; /*!< The network timeout in milliseconds */
	QTimer *mNetworkTimer; /*!< Holds a timer for current executing network  */

	/*! Main entry point for separate thread execution. Subclass should not call this function directly. */
	void run();
	/*! Inherited from base class, not used in current implementation. Subclass should not call this function directly. */
	TaskStatus execute() {throw std::exception(); };
	/*! Called before the task do any actual work. Subclass should not call this function directly.
	 * Please see @ref SFNetworkAccessTask_Subclass "Subclassing Notes" for more details. */
	virtual void prepare();
	/*! Called at the end of task. This function is responsible to cleanup memory, attempt re-try and emit signals. Subclass should not call this function directly.
	 * Please see @ref SFNetworkAccessTask_Subclass "Subclassing Notes" for more details. */
	virtual void cleanup();
	/*! Responsible to reset the task. Subclass should not call this function directly.
	 * Please see @ref SFNetworkAccessTask_Subclass "Subclassing Notes" for more details. */
	virtual bool retry();

	/*! Prepare and validate current request
	 * @return The state of the task:
	 * 	+ @c SFNetworkAccessTask::StateReadyToSend indicates that the request object is fully prepared and ready to be sent.
	 * 	+ @c SFNetworkAccessTask::StateError indicates that the request object is not ready and the operation should be aborted.
	 *
	 * Please see @ref SFNetworkAccessTask_Subclass "Subclassing Notes" for more details. */
	virtual NetworkTaskState ensureRequest();

	/*! Pre-porcess the reply and handles HTTP redirect.
	 * @return The state of the task:
	 * 	+ @c SFNetworkAccessTask::StateReadyToProcess indicates that the reply is ready to be processed.
	 * 	+ @c SFNetworkAccessTask::StateNeedToRedirect indicates that another request to server is needed. The new URL should be assigned to
	 * 	@c SFNetworkAccessTask::mRequest when the function returns.
	 * 	+ @c SFNetworkAccessTask::StateError indicates that the error is detected and the task should be abort.
	 *
	 * 	Please see @ref SFNetworkAccessTask_Subclass "Subclassing Notes" for more details. */
	virtual NetworkTaskState preprocessReply(QNetworkReply * reply);

	/*! Process the reply. This function is always called in separate thread.
	 * @return The state of the task:
	 * 	+ @c SFNetworkAccessTask::StateFinished indicates that the task is finished and the result is ready for delivery
	 * 	+ @c SFNetworkAccessTask::StateError indicates that the error is detected and the task should be abort.
	 * 	+ @c SFNetworkAccessTask::StateNeedToRetry indicates that error is detected and task should be restarted. Custom logic should check
	 * 	whether the re-try is enabled. It shouldn't return this state if re-try is not enabled.
	 *
	 * 	Please see @ref SFNetworkAccessTask_Subclass "Subclassing Notes" for more details. */
	virtual NetworkTaskState processReply(QNetworkReply * reply);

	//debug
	/*! Convenient function for debug purpose. Generate a nice formated summary of given reply's headers */
	QString composeReplyHeader(QNetworkReply * reply);
	/*! Convenient function for debug purpose. Generate a string expression of given state */
	QString stateDescription(NetworkTaskState state);

private:
	Q_INVOKABLE void moveQObjectsToThread(QThread *thread);
	Q_INVOKABLE void fsmDispatcher();
	NetworkTaskState initiateNetworkAccess();
	QNetworkReply * createReplyAndExit();

};
//typedef sf::SFNetworkAccessTask::HTTPMethod HTTPMethod;

} /* namespace */
#endif /* SFNETWORKACCESSTASK_H_ */
