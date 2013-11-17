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
* SFNetworkAccessTask.cpp
*
*  Created on: Apr 3, 2013
*      Author: Livan Yi Du
*/

#include "SFNetworkAccessTask.h"
#include <bb/data/JsonDataAccess>
#include <QThreadPool>
#include <QBuffer>
#include <QTimer>
#include "SFResult.h"

using namespace bb::data;

namespace sf {

static unsigned long int DefaultNetworkTimeout = 60000; // 1 minutes

SFNetworkAccessTask::SFNetworkAccessTask(QNetworkAccessManager *networkAccessManager)
: SFGenericTask(), mNetworkAccessManager(networkAccessManager), mRequest(QNetworkRequest(QUrl())), mMethod(HTTPMethod::HTTPGet),
  mRequestBytesArray(QByteArray()), mRequestData(NULL), mState(StateNotStarted), mCurrentReply(NULL),
  mUseCache(false), mNetworkTimeout(DefaultNetworkTimeout), mNetworkTimer(NULL) {

	this->setUseCache(false);
}

SFNetworkAccessTask::SFNetworkAccessTask(QNetworkAccessManager * const networkAccessManager, const QNetworkRequest & request, const HTTPMethodType & method)
: SFGenericTask(), mNetworkAccessManager(networkAccessManager), mRequest(request), mMethod(method),
  mRequestBytesArray(QByteArray()), mRequestData(NULL), mState(StateNotStarted), mCurrentReply(NULL),
  mUseCache(false), mNetworkTimeout(DefaultNetworkTimeout), mNetworkTimer(NULL) {

	this->setUseCache(false);
}

SFNetworkAccessTask::SFNetworkAccessTask(QNetworkAccessManager * const networkAccessManager, const QUrl & url, const HTTPMethodType & method)
: SFGenericTask(), mNetworkAccessManager(networkAccessManager), mRequest(QNetworkRequest(url)), mMethod(method),
  mRequestBytesArray(QByteArray()), mRequestData(NULL), mState(StateNotStarted), mCurrentReply(NULL),
  mUseCache(false), mNetworkTimeout(DefaultNetworkTimeout), mNetworkTimer(NULL) {

	this->setUseCache(false);
}

SFNetworkAccessTask::SFNetworkAccessTask(QNetworkAccessManager * const networkAccessManager, const QString & path, const HTTPMethodType & method)
: SFGenericTask(), mNetworkAccessManager(networkAccessManager), mRequest(QNetworkRequest(QUrl(path))), mMethod(method),
  mRequestBytesArray(QByteArray()), mRequestData(NULL), mState(StateNotStarted), mCurrentReply(NULL),
  mUseCache(false), mNetworkTimeout(DefaultNetworkTimeout), mNetworkTimer(NULL) {

	this->setUseCache(false);
}

SFNetworkAccessTask::~SFNetworkAccessTask() {

}

/*********************
 * accessors
 *********************/
void SFNetworkAccessTask::setRequestBytesArray(const QByteArray & bytes) {
	mRequestBytesArray = bytes;
	if (!bytes.isNull() && this->mRequestData != NULL) {
		this->mRequestData->deleteLater();
		this->mRequestData = NULL;
	}
};

void SFNetworkAccessTask::setRequestData(QIODevice * data) {
	if (data) {
		data->setParent(this);
		mRequestBytesArray = QByteArray();
	}
	this->mRequestData = data;
}

/* Overrides */
void SFNetworkAccessTask::startTaskAsync(QObject* resultReceiver, const char * resultReceiverSlot) {
	SFGenericTask::prepareToStart(resultReceiver, resultReceiverSlot);
	if (this->mCurrentReply) {
		this->mCurrentReply->deleteLater();
		this->mCurrentReply = NULL;
	}
	mState = StateNotStarted;
	this->fsmDispatcher();
}

void SFNetworkAccessTask::run() {
	try {
		//call the function in creator's thread and block
		bool ret = this->metaObject()->invokeMethod(this, "moveQObjectsToThread", Qt::BlockingQueuedConnection, Q_ARG(QThread*, QThread::currentThread()));
		if (!ret || !mCurrentReply) {
			mResult = mResult ? mResult : SFResult::createErrorResult(SFResultCode::SFErrorNetwork, "No reply to process.");
			mState = StateError;
		} else {
			mState = this->processReply(mCurrentReply);
		}

		mStatus = TaskStatusFinished;
	} catch(std::exception &e) {
		mStatus = TaskStatusError;
		sfWarning() << "[SFNetworkAccessTask] Exception:" << e.what();
		this->prepareQObjectForDisposal(mResult);
		mResult = mResult ? mResult : SFResult::createErrorResult(SFResultCode::SFErrorNetwork, QString(e.what()));
		mState = StateError;
	} catch (...) {
		mStatus = TaskStatusError;
		sfWarning() << "[SFNetworkAccessTask] Unknown Error";
		this->prepareQObjectForDisposal(mResult);
		mResult ? mResult : SFResult::createErrorResult(SFResultCode::SFErrorNetwork, QString("Fatal Error"));
		mState = StateError;
	}

	this->fsmDispatcher();
}

void SFNetworkAccessTask::prepare() {
	 SFGenericTask::prepare();
}

void SFNetworkAccessTask::cleanup() {
	if (mResult && mCurrentReply && mCurrentReply->thread() == mResult->thread()) {
		mCurrentReply->setParent(mResult); //the reply will be deleted when result is deleted
	} else if (mCurrentReply && mCurrentReply->thread() == this->thread()) {
		mCurrentReply->setParent(this); //the reply will be deleted when task is deleted
	} else if (mResult && mCurrentReply) {
		sfWarning() << "[SFNetworkAccessTask] QNetworkReply & SFResult created in different thread, potential memory leak.";
	}
	SFGenericTask::cleanup();
}

bool SFNetworkAccessTask::retry() {
	//close current data buffer
	if (this->mRequestData && !this->mRequestBytesArray.isNull()) {
		//the mRequestData was created using mRequestBytesArray, delete it;
		this->mRequestData->deleteLater();
		this->mRequestData = NULL;
	}
	return SFGenericTask::retry();
}

/* to be run in any thread */
void SFNetworkAccessTask::fsmDispatcher() {
	if (this->isCancelled()) {
		this->cleanup();
		return;
	}
	NetworkTaskState oldState = this->mState;
	switch(this->mState) {
	//following state may happen only in creator's thread
	case StateNotStarted:
		//start the task
		mStatus = TaskStatusRunning;
		this->prepare();
		mState = this->ensureRequest();
		break;

	case StateReadyToSend:
		mState = this->initiateNetworkAccess();
		break;

	case StateHasResponse:
		mState = this->preprocessReply(mCurrentReply);
		break;

	case StateNeedToRedirect:
		if (mCurrentReply) {
			mCurrentReply->deleteLater();
			mCurrentReply = NULL;
		}
		mState = this->initiateNetworkAccess();
		break;

	case StateReadyToProcess:
		QThreadPool::globalInstance()->start(this);
		break;

	//following state may happen in any thread
	case StateFinished:
		mStatus = TaskStatusFinished;
		this->cleanup();
		break;
	case StateError:
		mStatus = TaskStatusError;
		this->cleanup();
		break;
	case StateNeedToRetry:
		//cleanup will finish up the task if no retry is permitted
		mStatus = TaskStatusWillRetry;
		this->cleanup();
		break;
	default:
		return;
	}

	//if reach here, mState is changed and require immediate attention
	if (oldState != this->mState) {
		//for debug
//		sfDebug() << "[DEBUG] NetworkAccessTask: " << this->stateDescription(oldState)
//				<< " --> " << this->stateDescription(this->mState);

		//try to move to next state
		this->metaObject()->invokeMethod(this, "fsmDispatcher"); //call the function in creator's thread
	}
}

/* to be run in creator's thread */
void SFNetworkAccessTask::moveQObjectsToThread(QThread *thread) {
	if (thread == NULL) {
		return;
	}
	if (mCurrentReply != NULL) {
		mCurrentReply->setParent(0);
		mCurrentReply->moveToThread(thread);
	}
	if (mResult != NULL) {
		mResult->moveToThread(thread);
	}
}


SFNetworkAccessTask::NetworkTaskState SFNetworkAccessTask::ensureRequest() {
	//check request
	if (this->mRequest.url().isEmpty()) {
		mResult = mResult ? mResult : SFResult::createErrorResult(SFResultCode::SFErrorNetwork, "Empty URL.");
		return StateError;
	}

	//check request data
	if (!this->mRequestBytesArray.isNull()) {
		//create buffer for bytes array
		if (this->mRequestData) {
			this->mRequestData->deleteLater();
		}
		this->mRequestData = new QBuffer(&this->mRequestBytesArray, this);
	}

	//set some attributes
	if (mUseCache) {
		mRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
		mRequest.setAttribute(QNetworkRequest::CacheSaveControlAttribute, true);
	} else {
		mRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
		mRequest.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
	}
	return StateReadyToSend;

}

SFNetworkAccessTask::NetworkTaskState SFNetworkAccessTask::initiateNetworkAccess() {
	//ensure network
	mCurrentReply = this->createReplyAndExit();
	if (!mCurrentReply) {
		mResult ? mResult : SFResult::createErrorResult(SFResultCode::SFErrorNetwork, "Network resource is not available.");
		return StateError;
	} else {
		sfWarning() << "[SFNetworkAccessTask] Request Sent. ";
		connect(mCurrentReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
		//restart timer
		if (!mNetworkTimer) {
			mNetworkTimer = new QTimer(this);
			mNetworkTimer->setSingleShot(true);
			connect(mNetworkTimer, SIGNAL(timeout()), this, SLOT(onNetworkTimeout()));
		}
		mNetworkTimer->start(mNetworkTimeout);
		return StateWaiting;
	}
}

void SFNetworkAccessTask::onReplyFinished() {
	if (mNetworkTimer) {
		mNetworkTimer->stop();
	}
	this->mState = StateHasResponse;
	this->fsmDispatcher();
}

void SFNetworkAccessTask::onNetworkTimeout() {
	if (mCurrentReply) {
		//this will trigger onReplyFinished
		mCurrentReply->abort();
	}
}

QNetworkReply * SFNetworkAccessTask::createReplyAndExit() {
	QNetworkReply *reply = NULL;

	switch(this->mMethod) {
	case HTTPMethod::HTTPGet:
		reply = mNetworkAccessManager->get(this->mRequest);
		break;
	case HTTPMethod::HTTPHead:
		reply = mNetworkAccessManager->head(this->mRequest);
		break;
	case HTTPMethod::HTTPDelete:
		reply = mNetworkAccessManager->sendCustomRequest(this->mRequest, QString("DELETE").toUtf8(), this->mRequestData);
		break;
	case HTTPMethod::HTTPPost:
		reply = mNetworkAccessManager->post(this->mRequest, this->mRequestData);
		break;
	case HTTPMethod::HTTPPut:
		reply = mNetworkAccessManager->put(this->mRequest, this->mRequestData);
		break;
	case HTTPMethod::HTTPPatch:
		reply = mNetworkAccessManager->sendCustomRequest(this->mRequest, QString("PATCH").toUtf8(), this->mRequestData);
		break;
	default:
		break;
	}

	if (!reply && !mResult) {
		mResult = SFResult::createErrorResult(SFResultCode::SFErrorNetwork, "request is not sent, possibly unsupported method");
	}
	return reply;
}

SFNetworkAccessTask::NetworkTaskState SFNetworkAccessTask::preprocessReply(QNetworkReply * reply) {
	//check error
	if (!reply) {
		mResult = mResult ? mResult : SFResult::createErrorResult(SFResultCode::SFErrorGeneric, "Internal Error: QNetworkReply is NULL.");
		return StateError;

	} else if (reply->error() != QNetworkReply::NoError) {
		//if there is error, we simply don't handle redirect and leave it up to processing routine
	}

	//check if redirect
	QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if (reply->error() == QNetworkReply::NoError && redirect.type() != QVariant::Invalid) {
		//handle redirect, delete old url & request, create new ones
		sfWarning() << "[SFNetworkAccessTask] Redirect to " << redirect.value<QUrl>();
		QUrl url = QUrl(redirect.value<QUrl>());
		this->mRequest.setUrl(url);

		return StateNeedToRedirect;
	}

	QVariant fromCache = reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute);
	if (fromCache.toBool()) {
		sfDebug() << "[SFNetworkAccessTask] Used response From cache. URL:" << reply->url().path();
	}

	return StateReadyToProcess;
}

SFNetworkAccessTask::NetworkTaskState SFNetworkAccessTask::processReply(QNetworkReply * reply) {
	if (reply->error() != QNetworkReply::NoError) {
		//try to use status code
		bool hasStatusCode = false;
		int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(&hasStatusCode);
		QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

		//for debug
		sfDebug() << "Network error. HTTP response:" << statusCode << "-" << reason << "QT response:" << reply->error() << "-" << reply->errorString();

		mResult = mResult ? mResult : SFResult::createErrorResult(hasStatusCode ? statusCode : reply->error(), reason.isNull() || reason.isEmpty() ? reply->errorString() : reason);

		return StateError;
	}

	mResult = SFResult::create();
	mResult->mPayload = QVariant::fromValue<QObject*>(reply);
	mResult->mStatus = SFResult::TaskResultSuccess;
	return StateFinished;
}

QString SFNetworkAccessTask::composeReplyHeader(QNetworkReply * reply) {
	QList<QNetworkReply::RawHeaderPair> headers = reply->rawHeaderPairs();
	QString headerText = "";
	for(int i=0; i<headers.size(); i++) {
		QNetworkReply::RawHeaderPair headerPair = headers[i];
		headerText = headerText + "\n" + QString(headerPair.first) + " = " + QString(headerPair.second);
	}
	return headerText;
}

QString SFNetworkAccessTask::stateDescription(NetworkTaskState state) {
	switch(state) {
	case StateNotStarted:
		return "Not Started";
	case StateReadyToSend:
		return "Ready to Send";
	case StateWaiting:
		return "Waiting";
	case StateHasResponse:
		return "Response Ready";
	case StateNeedToRedirect:
		return "NeedToRedirect";
	case StateReadyToProcess:
		return "Ready to Process";
	case StateProcessing:
		return "Processing";
	case StateFinished:
		return "Finished";
	case StateError:
		return "Error";
	case StateNeedToRetry:
		return "Need to Retry";
	default:
		return "Unknown";
	}
}

} /* namespace */
