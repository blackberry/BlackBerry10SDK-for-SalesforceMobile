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

class SFGenericTask: public QObject, public QRunnable {
	Q_OBJECT
	Q_ENUMS(TaskStatus)
	Q_PROPERTY(TaskStatus status READ status);
	Q_PROPERTY(sf::SFResult* result READ result);
	Q_PROPERTY(QVariantHash tags READ tags);
	Q_PROPERTY(bool cancellable READ isCancellable WRITE setCancellable);
	Q_PROPERTY(bool autoRetry READ isAutoRetry WRITE setAutoRetry);

signals:
	void taskPreExecution(sf::SFGenericTask*);
	void taskFinished(sf::SFGenericTask*);
	void taskResultReady(sf::SFResult*);
	void taskShouldRetry(sf::SFGenericTask*, sf::SFResult*);
	void taskDidAutoRetry(sf::SFGenericTask*);
public:
	enum TaskStatus {
		TaskStatusNotStarted = -2,
		TaskStatusRunning = -1,
		TaskStatusFinished= 0,
		TaskStatusError = 1,
		TaskStatusCancelled = 2,
		TaskStatusWillRetry = 3,
	};
	SFGenericTask(const bool & cancellable = false);
	virtual ~SFGenericTask();
	void run();
	TaskStatus status() {return mStatus;};
	SFResult *result() {return mResult;};
	QVariantHash & tags() { return mTags;};
	bool isCancellable() {return mCancellable;};
	void setCancellable(const bool & cancellable);
	bool isAutoRetry() {return mAutoRetry;};
	void setAutoRetry(const bool & autoRetry) {this->mAutoRetry = autoRetry;};
	int retryCount() {return mRetryCount;};
	void setRetryCount(const int & retryCount) {this->mRetryCount = retryCount;};
	void putTag(const QString & key, const QVariant & tag) {mTags[key] = tag;};

public slots:
	virtual void startTaskAsync(QObject* resultReceiver = NULL, const char * resultReceiverSlot = NULL);
	virtual void cancel();

protected:
	volatile TaskStatus mStatus;
	SFResult *mResult;
	QVariantHash mTags;
	QThread * mExecutionThread;
	bool mCancellable;
	bool mAutoRetry;
	int mRetryCount;
	QEventLoop* eventLoop();
	QMutex *mutex();
	void setCancelled(const bool &cancelled);
	bool isCancelled();

	virtual void prepareToStart(QObject* receiver, const char * slot);
	virtual void prepare();
	virtual TaskStatus execute() = 0;
	virtual void cleanup();
	virtual bool retry();

	/* this function assume the given object lives in current executing thread */
	void prepareQObjectForDisposal(QObject* object);
private:
	QEventLoop *mEventLoop;
	QMutex *mMutex;
	bool mCancelled;
};

}/* namespace */
#endif /* SFGENERICTASK_H_ */
