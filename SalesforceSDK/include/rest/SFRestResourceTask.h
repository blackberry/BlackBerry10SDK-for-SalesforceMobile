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
* SFRestResourceTask.h
*
* required library:
* LIBS += -lbbdata
*  Created on: Mar 6, 2013
*      Author: Livan Yi Du
*/

#ifndef RESTRESOURCETASK_H_
#define RESTRESOURCETASK_H_

#include "SFNetworkAccessTask.h"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include "SFRestRequest.h"

namespace sf {

/*!
 * @class SFRestResourceTask
 * @headerfile SFRestResourceTask.h <rest/SFRestResourceTask.h>
 * @brief The SFRestResourceTask is designed to execute REST transactions and handle the server response.
 *
 * @details
 * This class takes a @c SFRestRequest object, sends it using QtNetwork and handles and delivers the result. Each instance of this class
 * represent a REST transaction between client and server applications.
 *
 * The class performs all operations in asynchronous fashion. All network communications are handled by QtNetwork which is already
 * asynchronous. All response parsing and error handling are performed in separate threads which are managed by @c QThreadPool. The class
 * uses QT's signal-slot framework to deliver results and notify about the progress.
 *
 * The class automatically claim ownership of the associated SFRestRequest and any SFResult object it created. It will delete itself upon
 * the completion of the task.
 *
 *
 * Usage
 * ------
 * Typically, developers don't need to deal with this class directly. To send REST request, @c SFRestAPI provides an convenient function SFRestAPI::sendRestRequest()
 * which internally uses this class. However, the use of this class is fairly simple:
 *
 * @code
 * SFRestRequest * request;
 *
 * //setup the request
 *
 * //create the task
 * SFRestResourceTask *task = new SFRestResourceTask(getSharedNetworkAccessManager(), request);
 *
 * //connect signals for result delivery.
 * connect(task, SIGNAL(taskResultReady(sf::SFResult*)), this, SLOT(onTaskResultReady(sf::SFResult*)));
 *
 * //start the task.
 * task->startTaskAsync();
 * @endcode
 *
 * By default, the class disables the re-try feature. You could enable it before
 * starting the task:
 * @code
 * task->setAutoRetry(true);
 * task->setRetryCount(1);
 * @endcode
 * With code above, the task will automatically re-try once if it detect any error.
 *
 * If you want to have some controlled re-try routine:
 * @code
 * task->setAutoRetry(false);
 * task->setRetryCount(1);
 * connect(task, SIGNAL(taskShouldRetry(sf::SFGenericTask*, sf::SFResult*)), this, SLOT(onTaskShouldRetry(sf::SFGenericTask*, sf::SFResult*)));
 * @endcode
 * And your re-try slot could look like:
 * @code
 * void SFRestAPI::onTaskShouldRetry(SFGenericTask* genericTask, SFResult* result) {
 *
 * 	int currentCondition;
 *
 * 	//check for current condition, e.g. if network is available, or if authentication is required.
 *
 * 	switch(currentCondition) {
 * 	case RetryImmediately:
 * 		QMetaObject::invokeMethod(genericTask, "startTaskAsync", Qt::QueuedConnection);
 * 		break;
 * 	case EnqueAndRetryLater:
 * 		genericTask->setParent(this);
 * 		someTaskQueue.enqueue(genericTask);
 * 		break;
 * 	case CannotRetry:
 * 	default:
 * 		genericTask->deleteLater(); //it's handler's responsibility to delete it in this case.
 * 		break;
 * 	}
 * }
 * @endcode
 * @note The implementation guarantees that @a genericTask is a valid pointer to a SFGenericTask object at the moment when slots are called.
 * It also assume that the ownership of the task will be transfered to whatever handling the re-try. It will re-claim the ownership
 * when @c startTaskAsync() is executed. Therefore, if you decide not to re-try, make sure you delete the task manually.
 * Otherwise, it leads to memory leak.
 *
 * Subclassing Notes
 * -----------------
 *
 * Although this class is designed to fit most common needs, you could extend it for better control over the REST request or additional response data
 * processing. This class inherited the Finite-State-Machine (FSM) implementation from its base class @c SFNetworkAccessTask. Each overridable
 * function is responsible to handle one step within the entire transaction. When finished, each of these functions returns a new "state"
 * based on the progress. The dispatcher function is in charge of calling these functions at proper time and certain "state". Please see
 * @c SFNetworkAccessTask for more information. You could find all possible states in @c SFNetworkAccessTask::NetworkTaskState.
 *
 * The class declared two functions as "virtual":
 *
 * If you want to implement customized request, you should override @c ensureRequest(). This function is called from main thread (task's
 * creator thread) immediately before the request is sent. The FSM dispatcher expects that @c SFNetworkAccessTask::mRequest is holding a
 * valid @c QNetworkRequest object after this function returns. The function could return following state:
 * 	- @c SFNetworkAccessTask::StateReadyToSend indicates that the request object is fully prepared and ready to be sent.
 * 	- @c SFNetworkAccessTask::StateError indicates that error occurred while preparing the request, the task should be aborted.
 * 	The FSM dispatcher also expects @c SFGenericTask::mResult is created with error code and message.
 * 	- @c SFNetworkAccessTask::StateNeedToRetry indicates that error occurred and re-try is permitted. Subclass should check retry
 * 	task's re-try configuration before returning this state code.
 *
 * Example:
 * @code
 * SFNetworkAccessTask::NetworkTaskState MyRestTask::ensureRequest() {
 *
 * 	bool hasError = false;
 * 	//do something with this->mRestRequest
 * 	//...
 * 	if(hasError) {
 * 		this->mResult = mResult ? mResult : SFResult::createErrorResult(SFResultCode::SFErrorGeneric, "custom error message");
 * 		return SFNetworkAccessTask::StateError;
 * 	}
 *
 * 	SFNetworkAccessTask::NetworkTaskState state = SFRestResourceTask::ensureRequest();
 * 	if (state == SFNetworkAccessTask::StateError || state == SFNetworkAccessTask::StateNeedToRetry) {
 * 		return state; //this->mResult is created by base implementation
 * 	}
 *
 * 	//do something with this->mRequest
 * 	//...
 * 	if(hasError) {
 * 		this->mResult = mResult ? mResult : SFResult::createErrorResult(SFResultCode::SFErrorGeneric, "custom error message");
 * 		return SFNetworkAccessTask::StateError;
 * 	}
 *
 * 	return SFNetworkAccessTask::StateReadyToSend
 * }
 * @endcode
 *
 * <br>
 * Sometimes you want to add additional data handling. For example, you want to further process/filer the data and save
 * it to disk or populate it in QObjects for easier property binding. You could override @c parseJsonContent(). This function is
 * called from a separate thread after JSON response is parsed. The function takes one parameter which is the direct output of
 * @c JsonDataAccess::load(), and possible returned states are:
 * 	- @c SFNetworkAccessTask::StateFinished indicates that the transaction is finished without error and @c SFGenericTask::mResult
 * 	is ready for delivery.
 * 	- @c SFNetworkAccessTask::StateError indicates that error occurred and the task should be aborted.
 * 	The FSM dispatcher also expects @c SFGenericTask::mResult is created with error code and message.
 * 	- @c SFNetworkAccessTask::StateNeedToRetry indicates that error occurred and re-try is permitted. Subclass should check retry
 * 	task's re-try configuration before returning this state code.
 *
 * @note At the entry point of this function, the overriding function shouldn't assume the existence of @c mResult object, neither
 * should it assume that the network response contains success status code. The function should check current @c mResult object
 * for any server side errors.
 *
 * @note However, it can assume that the input parameter "jsonContent" is parsed from HTTP response body and is valid
 * QVariantMap or QVariantList. The HTTP status code and QNetworkReply error code has been processed when this function is called.
 *
 * \sa SFRestAPI, SFRestRequest, SFNetworkAccessTask, SFGenericTask
 *
 * \author Livan Yi Du
 */
class SFRestResourceTask : public sf::SFNetworkAccessTask {
	Q_OBJECT
public slots:

public:
	/*! Create a REST task with prepared SFRestRequest
	 * @param networkAccessManager the shared QNetworkAccessManager. The task will use it to send the request
	 * @param request a pointer to a SFRestRequest. The task will take ownership of the given request if not NULL. */
	SFRestResourceTask(QNetworkAccessManager * const networkAccessManager, SFRestRequest * request = NULL);

	/*! destructor */
	virtual ~SFRestResourceTask();

protected:
	/*! Prepare and validate @c SFNetworkAccessTask::mRequest. Called immediately before the request is sent
	 * @return the state of the task */
	virtual NetworkTaskState ensureRequest();

	/*! Parse and process JSON object(s) returned from server.
	 * @note Override notice: the overriding function shouldn't assume the existence of mResult object, neither
	 * should it assume that the network response contains success status code. The overriding function
	 * should check current mResult object for any server side errors.
	 * However, it can assume that the "jsonContent" is parsed from HTTP response body and is valid
	 * QVariantMap or QVariantList. The HTTP status code and QNetworkReply error code has been processed
	 * when this function is called.
	 * @param jsonContent output of JsonDataAccess::load(), contains QVariantMap or QVariantList
	 * @return the state of the task after parsing */
	virtual NetworkTaskState parseJsonContent(const QVariant & jsonContent);

	/*! Exam the network reply and parse it to JSON objects if possible */
	NetworkTaskState processReply(QNetworkReply * reply);

	SFRestRequest *mRestRequest; /*! The pointer to a @c SFRestRequest instance */

private:
	NetworkTaskState processStatusCode(const int & statusCode, const QString & reason, QNetworkReply * reply);
	NetworkTaskState processNetworkErrorCode(const int & errorCode, const QString & reason);
};

} /* namespace sf */
Q_DECLARE_METATYPE(sf::SFRestResourceTask*)
#endif /* RESTRESOURCETASK_H_ */
