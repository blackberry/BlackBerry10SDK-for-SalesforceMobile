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
* SFGenericTask.h
*
*  Created on: Mar 6, 2013
*      Author: Livan Yi Du
*/

#ifndef SFGENERICTASK_H_
#define SFGENERICTASK_H_

#include <QObject>
#include <QRunnable>
#include <QVariant>

class QEventLoop;
class QMutex;

namespace sf {
class SFResult;

using namespace Qt;

/*!
 * @class SFGenericTask
 * @headerfile SFGenericTask.h <core/SFGenericTask.h>
 * @brief An abstract class that defines a concurrent Task-Oriented logic block
 *
 * @details The purpose of this class is to establish a task-oriented logic block which can be easily executed concurrently. The class inherits
 * from @c QRunnable which can be executed in separate thread. It also implemented @c QObject features in order to take the advantage of
 * event loop and signal-slot architecture.
 *
 * The class defined numbers of properties and signals for reporting progress and controlling work flow. It also provides various of virtual
 * functions for subclass to override. In addition, the class implemented numbers of convenient functions to create/manage objects that
 * is commonly used in multi-threading environment.
 *
 * To create a simple concurrent task, all you need to do is to create a subclass of @c SFGenericTask and implement the @c execute(). Within your
 * @c execute() function, make sure you populate the @c SFGenericTask::mResult object and return proper status code.
 *
 * When you want to execute your concurrent task, create an instance of your subclass and call @c startTaskAsync(). The base class implementation
 * makes sure that the logic in @c execute() is executed in separate thread and the result object you created in that function is moved to
 * correct thread when it's delivered.
 *
 * It is up to developers on how to use this class. @c SFNetworkAccessTask is a concrete implementation of this class.
 * It demonstrates that you could create a much more complex operation out of @c SFGenericTask by overriding functions.
 *
 * @see SFResult, SFNetworkAccessTask, SFRestResourceTask
 *
 * \author Livan Yi Du
 */
class SFGenericTask: public QObject, public QRunnable {
	Q_OBJECT
	Q_ENUMS(TaskStatus)
	Q_PROPERTY(TaskStatus status READ status) /*!< The current status of the task. @sa SFGenericTask::mStatus, SFGenericTask::TaskStatus */
	Q_PROPERTY(sf::SFResult* result READ result) /*!< The result object of this task @sa result(), SFGenericTask::mResult*/
	Q_PROPERTY(QVariantHash tags READ tags) /*!< A hash map of any additional objects associated with the task. @sa tags(), putTag() */
	Q_PROPERTY(bool cancellable READ isCancellable WRITE setCancellable) /*!< Whether this task can be cancelled. Default is @a false. @sa isCancellable(), setCancellable() */
	Q_PROPERTY(bool autoRetry READ isAutoRetry WRITE setAutoRetry) /*!< Whether the auto-retry is enabled. @sa setAutoRetry(), isAutoRetry() */

signals:
	/*! Emitted before the @c execute() is invoked.
	 * @param task the task that emitted the signal. */
	void taskPreExecution(sf::SFGenericTask* task);
	/*! Emitted after the task is finished.
	 * @param task the task that is finished. @note The implementation only guarantee that the task object is valid at the time when
	 * slots are invoked. @c deleteLater() is called after the signal is emitted. So attempting to dereference the pointer at later
	 * time may cause memory access error.*/
	void taskFinished(sf::SFGenericTask* task);
	/*! Emitted when result is ready
	* @param result the result object. @note the object is only valid within the scope of connected slot. It will be deleted when
	* all connected slots are finished. So you have to collect all data before the @a result object get deallocated. */
	void taskResultReady(sf::SFResult* result);
	/*! Emitted when manual retry is required. It's connected slot's responsibility to re-start the task or delete the task.
	 * @param task the task that want to be re-started
	 * @param result the object that contains the reason of re-trying.*/
	void taskShouldRetry(sf::SFGenericTask* task, sf::SFResult* result);
	/*! Emitted when auto retry happened
	 * @param task the task that re-started itself*/
	void taskDidAutoRetry(sf::SFGenericTask* task);
public:
	/*! Possible status */
	enum TaskStatus {
		TaskStatusNotStarted = -2, /*!< The task is not started yet */
		TaskStatusRunning = -1, /*!< The task is executing */
		TaskStatusFinished= 0, /*!< The task is finished without error */
		TaskStatusError = 1, /*!< The task is finished with error */
		TaskStatusCancelled = 2, /*!< The task is canceled */
		TaskStatusWillRetry = 3, /*!< The task will be re-started */
	};
	/*! @param cancellable whether the task can be canceled */
	SFGenericTask(const bool & cancellable = false);
	/*! Default destructor */
	virtual ~SFGenericTask();
	/*! Entry point of concurrent execution. Subclass should not call this function directly. */
	void run();
	/*! @return status of the task */
	TaskStatus status() {return mStatus;};
	/*! @return the result object of the task */
	SFResult *result() {return mResult;};
	/*! @return the hash map of all additional objects associated with this task. @sa putTag()*/
	QVariantHash & tags() { return mTags;};
	/*! @return Whether this task can be canceled. @sa setCancellable(), SFGenericTask::cancellable */
	bool isCancellable() {return mCancellable;};
	/*! Set whether this task can be canceled. This flag should be set before the task is started. @sa isCancellable(), SFGenericTask::cancellable */
	void setCancellable(const bool & cancellable);
	/*! @return Whether auto-retry is enabled. @sa setAutoRetry(), SFGenericTask::autoRetry */
	bool isAutoRetry() {return mAutoRetry;};
	/*! Set whether auto-retry is enabled. If enabled, the task would re-start itself and decrement @c SFGenericTask::retryCount() by 1, whenever error occurred. @sa isAutoRetry(), SFGenericTask::autoRetry */
	void setAutoRetry(const bool & autoRetry) {this->mAutoRetry = autoRetry;};
	/*! @return how many re-try attempts are allowed. @sa setRetryCount() */
	int retryCount() {return mRetryCount;};
	/*! Set how many re-try attempts are allowed. Setting this property after the task started result in undefined behavior. @sa retryCount()*/
	void setRetryCount(const int & retryCount) {this->mRetryCount = retryCount;};
	/*! Add an @c QVariant to the task and associate it with a key. @sa tags(), SFGenericTask::tags */
	void putTag(const QString & key, const QVariant & tag) {mTags[key] = tag;};

public slots:
	/*! Start the task asynchronously and connect a result handler to it */
	virtual void startTaskAsync(QObject* resultReceiver = NULL, const char * resultReceiverSlot = NULL);
	/*! Request to cancel the task. If @c SFGenericTask::cancellable is set to false, calling this function have no effect */
	virtual void cancel();

protected:
	volatile TaskStatus mStatus; /*!< Current task status */
	SFResult *mResult; /*!< The result object */
	QVariantHash mTags; /*!< A hash map holds all objects that related to the task. This collection will be transfered to @c SFResult at the end of the task. */
	QThread * mExecutionThread; /*!< The thread that execute the task. */
	bool mCancellable; /*!< flag of whether the task can be canceled */
	bool mAutoRetry; /*!< flag of whether the task should automatically re-try */
	int mRetryCount; /*!< allowed number of re-try attempts */
	QEventLoop* eventLoop(); /*!< Create or re-use a @c QEventLoop. @return For each task, it guarantees to return the same event loop object. */
	QMutex *mutex(); /*!< Create or re-use a @c QMutex. @return For each task, it guarantees to return the same mutex object. */
	void setCancelled(const bool &cancelled); /*!< Set a flag that indicating the task is requested to cancel. This is a thread-safe function. @sa isCancelled() */
	bool isCancelled(); /*!< @return whether the task is requested to cancel. Subclass should constantly check this flag if it support canceling. This is a thread-safe function. @sa setCancelled() */

	/*! Called before the task is added to thread pool. The base implementation responsible to connect result handling slot
	 * and set necessary properties for proper memory management. Subclass should always call base implementation.
	 * @param receiver The QObject that will handle the result
	 * @param slot The Qt slot that respond to @c taskResultReady(sf::SFResult*). The string should be generated using macro @c SLOT()*/
	virtual void prepareToStart(QObject* receiver, const char * slot);
	virtual void prepare(); /*!< Called in execution thread before the @c execute(). */
	/*! Pure member function. The subclass should override this function. Guaranteed called in execution thread and is try-catch protected
	 * @return the status of the task after the execution. Please see @c SFGenericTask::TaskStatus for details.*/
	virtual TaskStatus execute() = 0;
	virtual void cleanup(); /*!< Called after the @c execute(). The base implementation is responsible to clean up the memory, re-try and emit signals. Subclass should always call base.*/
	virtual bool retry(); /*!< Called when retry is necessary. This function is responsible to reset the task for re-start. @return whether the task is re-started.*/

	/*! This is a convenient function to push the object to task object's thread and set the task object as the parent. After calling this function.
	 * it's safe to remove references to the given object without potential memory leak. @note This function is usually called in execution thread, and the
	 * function assume the given object was created in the current executing thread.
	 * @param object the object you want to get rid of.*/
	void prepareQObjectForDisposal(QObject* object);
private:
	QEventLoop *mEventLoop;
	QMutex *mMutex;
	bool mCancelled;
};

}/* namespace */
#endif /* SFGENERICTASK_H_ */
