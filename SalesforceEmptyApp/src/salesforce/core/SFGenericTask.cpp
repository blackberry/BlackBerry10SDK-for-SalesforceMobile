/*
 * SFGenericTask.cpp
 *
 *  Created on: Mar 6, 2013
 *      Author: Livan Yi Du
 */

#include "SFGenericTask.h"
#include <QEventLoop>
#include <QThreadPool>
#include <QMutexLocker>
#include <QMetaObject>
#include <QEventLoop>
#include <QThread>
#include <QMutex>
#include "SFResult.h"


namespace sf {

SFGenericTask::SFGenericTask(const bool & cancellable) : QObject(), QRunnable() {
	mStatus = TaskStatusNotStarted;
	mResult = NULL;
	mEventLoop = NULL;
	mExecutionThread = NULL;
	mMutex = cancellable ? this->mutex() : NULL;
	mCancellable = cancellable;
	mCancelled = false;
	mAutoRetry = false;
	mRetryCount = 0; //no retry
	this->setAutoDelete(false);
}

SFGenericTask::~SFGenericTask() {
	delete mEventLoop;
	delete mMutex;
}

/*
 * Public
 */
void SFGenericTask::startTaskAsync(QObject* receiver, const char * slot) {
	this->prepareToStart(receiver, slot);
	QThreadPool::globalInstance()->start(this);
}

void SFGenericTask::setCancellable(const bool & cancellable) {
	if (cancellable) {
		//prepare mutex lock
		this->mutex();
	}
	this->mCancellable = cancellable;
}

void SFGenericTask::run() {
	mStatus = TaskStatusRunning;
	mExecutionThread = QThread::currentThread();
	this->prepare();

	try {

		mStatus = this->execute();

	} catch(std::exception &e) {
		mStatus = TaskStatusError;
		sfWarning() << "[SFGenericTask] Exception:" << e.what();
		this->prepareQObjectForDisposal(mResult);
		mResult = SFResult::create();
		mResult->mStatus = SFResult::TaskResultError;
		mResult->mCode = -1;
		mResult->mMessage = QString(e.what());
	} catch (...) {
		mStatus = TaskStatusError;
		sfWarning() << "[SFGenericTask] Unknown Error";
		this->prepareQObjectForDisposal(mResult);
		mResult = SFResult::create();
		mResult->mStatus = SFResult::TaskResultError;
		mResult->mCode = -1;
		mResult->mMessage = QString("Fatal Error");

	}

	this->cleanup();
}

void SFGenericTask::cancel() {
	//garuantee cancel is called from owner thread
	if (QThread::currentThread() != this->thread()) {
		QMetaObject::invokeMethod(this, "cancel");
		return;
	}

	//this one is typically called from thread other than the one this runnable belongs to (through meta obj invokation or signal)
	if (!mCancellable) {
		return;
	}
	sfWarning() << "Canceling task:"<< this;
	//set flag, cleanup() will check for it
	this->setCancelled(true);
}

/*
 * Protected
 */
void SFGenericTask::prepareToStart(QObject* receiver, const char * slot) {
	// we clear the parent because we want to keep the option to move the task to another thread
	// we shouldn't set parent anyway if the runnable have "auto delete" on
	this->setParent(0);
	this->setAutoDelete(false);
	if (this->mResult) {
		this->mResult->deleteLater();
		this->mResult = NULL;
	}
	if (receiver && slot) {
		connect(this, SIGNAL(taskResultReady(sf::SFResult*)), receiver, slot);
	}
}


QEventLoop* SFGenericTask::eventLoop() {
	if (!mEventLoop) {
		//we cannot assign "this" as parent because runnable is not created in same thread
		//so, remember to delete it!
		mEventLoop = new QEventLoop(0);
	}
	return mEventLoop;
}

QMutex* SFGenericTask::mutex() {
	if (!mMutex) {
		mMutex = new QMutex();
	}
	return mMutex;
}

void SFGenericTask::setCancelled(const bool &cancelled) {
	if (!mCancellable) {
		return;
	}
	QMutexLocker locker(mutex());
	mCancelled = cancelled;
}

bool SFGenericTask::isCancelled() {
	if (!mCancellable) {
		return false;
	}
	QMutexLocker locker(mutex());
	return mCancelled;
}

void SFGenericTask::prepare() {
	emit taskPreExecution(this);
}

void SFGenericTask::cleanup() {
	this->prepareQObjectForDisposal(mResult);
	if (this->isCancelled()) {
		mResult = SFResult::createCancelResult();
		mStatus = TaskStatusCancelled;
	} else if (!mResult) {
		mResult = SFResult::createErrorResult(SFResultCode::SFErrorGeneric, "No result.");
		mStatus = TaskStatusError;
	}
	mResult->mTags = mTags;

	if (mStatus != TaskStatusWillRetry ||  !this->retry()) {
		//no need to retry or can't retry, finish up
		emit taskResultReady(mResult);

		emit taskFinished(this);
		this->deleteLater();
	}
}

bool SFGenericTask::retry() {

	if (mRetryCount-- <= 0) {
		mStatus = TaskStatusError;
		return false;
	}

	emit taskShouldRetry(this, mResult);
	if (!mAutoRetry) {
		//we've done everything we could, now it's up to "taskShouldRetry" handlers to manually restart the task
		return true;
	}

	//up to this point, we assume the task has prepared itself for re-execution
	QMetaObject::invokeMethod(this, "startTaskAsync", Qt::QueuedConnection);

	emit taskDidAutoRetry(this);
	return true;
}

void SFGenericTask::prepareQObjectForDisposal(QObject* object) {
	if (!object) {
		return;
	}

	if (object->thread() != QThread::currentThread()) {
		sfWarning() << "[SFGenericTask] Failed to dispose object:" << object << ". Potential memory issue. Deleting now...";
		object->deleteLater();
		return;
	}
	QThread *creatorThread = this->thread();
	//try to push result to creators thread
	if (object->thread() != creatorThread) {
		object->moveToThread(creatorThread);
	}
	if (object->thread() == creatorThread) {
		object->setParent(this);
	}
}

}/* namespace */
