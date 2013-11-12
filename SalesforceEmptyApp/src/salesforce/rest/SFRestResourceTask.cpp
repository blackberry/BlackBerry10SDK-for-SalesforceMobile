/*
 * SFRestResourceTask.cpp
 *
 *  Created on: Mar 6, 2013
 *      Author: Livan Yi Du
 */

#include "SFRestResourceTask.h"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <bb/data/JsonDataAccess>
#include <QStringList>
#include "SFGlobal.h"
#include "SFResult.h"

using namespace bb::data;

namespace sf {

SFRestResourceTask::SFRestResourceTask()
: SFNetworkAccessTask(NULL) {
	this->mRestRequest = NULL;
}

SFRestResourceTask::SFRestResourceTask(QNetworkAccessManager *networkAccessManager)
: SFNetworkAccessTask(networkAccessManager) {
	this->mRestRequest = NULL;
}

SFRestResourceTask::SFRestResourceTask(QNetworkAccessManager * const networkAccessManager, SFRestRequest * request)
: SFNetworkAccessTask(networkAccessManager) {
	this->mRestRequest = request;
	if (request) {
		request->setParent(this);
	}

}

SFRestResourceTask::~SFRestResourceTask() {

}

SFNetworkAccessTask::NetworkTaskState SFRestResourceTask::ensureRequest() {
	if (this->mRestRequest == NULL) {
		this->mResult = mResult ? mResult : SFResult::createErrorResult(SFResultCode::SFErrorGeneric, "Did you forget to set SFRestRequest?");
		return SFNetworkAccessTask::StateError;
	}

	QString errorMsg;
	this->mRequest = QNetworkRequest();
	this->mRequestBytesArray = QByteArray();
	SFResultCodeType errorCode = this->mRestRequest->prepareNetworkRequest(&mRequest, &mRequestBytesArray, &errorMsg);
	if (errorCode != 0) {
		this->mResult = mResult ? mResult : SFResult::createErrorResult(errorCode, errorMsg);
		return SFNetworkAccessTask::StateError;
	}
	this->mMethod = this->mRestRequest->method();

	return SFNetworkAccessTask::ensureRequest();
}


SFNetworkAccessTask::NetworkTaskState SFRestResourceTask::processReply(QNetworkReply * reply) {
	bool hasStatusCode = false;
	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(&hasStatusCode);
	QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
	QByteArray buffer = reply->readAll();

	//process status code or error code
	NetworkTaskState state;
	if (hasStatusCode) {
		state = this->processStatusCode(statusCode, reason, reply);
	} else {
		state = this->processNetworkErrorCode(reply->error(), reply->errorString());
	}

	//for debug
	sfDebug() << "[SFRestResourceTask] Response Summary:"<< (hasStatusCode ? statusCode : reply->error()) << "-"
			<< ((reason.isNull() || reason.isEmpty()) ? reply->errorString() : reason)
			<< "\nHeader: \n" << this->composeReplyHeader(reply) << "\nContent: \n" << QString(buffer);

	//parse json
	JsonDataAccess jda;
	QVariant contentObj = jda.loadFromBuffer(buffer);
	if (jda.hasError()) {
		//check if the content is empty
		QString content = QString(buffer);
		if (content.isNull() || content.trimmed().isEmpty()) {
			//no content, we rely on status code and reason phrase
			return state;
		} else {
			//parsing error
			QString err = jda.error().errorMessage();
			int code = jda.error().errorType();
			mResult = mResult ? mResult : SFResult::createErrorResult(code, err);
			return StateError;
		}
	} else if (this->parseJsonContent(contentObj) == StateError) {
		return StateError;
	} else {
		return state;
	}
}

SFNetworkAccessTask::NetworkTaskState SFRestResourceTask::processStatusCode(const int & statusCode, const QString & reason, QNetworkReply * reply) {
	bool hardcodedReason = (reason.isNull() || reason.trimmed().isEmpty());
	bool isError = true;
	bool needToRetry = false;
	QString message;
	switch(statusCode) {
	case SFResultCode::SFRestStatusSuccess:
		message = (hardcodedReason) ? "Success." : reason;
		isError = false;
		break;
	case SFResultCode::SFRestStatusSuccessCreated:
		message = (hardcodedReason) ? "Created." : reason;
		isError = false;
		break;
	case SFResultCode::SFRestStatusSuccessNoContent:
		message = (hardcodedReason) ? "Success without no content." : reason;
		isError = false;
		break;
	case SFResultCode::SFRestStatusMultiRecords:
		message = (hardcodedReason) ? "Multiple records with given external ID were found." : reason;
		break;
	case SFResultCode::SFRestStatusNoChanges:
		message = (hardcodedReason) ? "The requested resource has not changed since the specified date and time." : reason;
		break;
	case SFResultCode::SFRestStatusBadData:
		message = (hardcodedReason) ? "Invalid request content." : reason;
		break;
	case SFResultCode::SFRestStatusInvalidSession:
		message = (hardcodedReason) ? "The session ID or OAuth token used has expired or is invalid." : reason;
		needToRetry = true;
		break;
	case SFResultCode::SFRestStatusRefused:
		message = (hardcodedReason) ? "The request has been refused." : reason;
		break;
	case SFResultCode::SFRestStatusNotFound:
		message = (hardcodedReason) ? "The requested resource couldn’t be found." : reason;
		break;
	case SFResultCode::SFRestStatusMethodNotAllowed:
		message = (hardcodedReason) ? "The method specified in the Request-Line isn’t allowed for the resource specified in the URI." : reason;
		break;
	case SFResultCode::SFRestStatusFormatNotSupported:
		message = (hardcodedReason) ? "The entity in the request is in a format that’s not supported by the specified method." : reason;
		break;
	case SFResultCode::SFRestStatusServerError:
		message = (hardcodedReason) ? "An error has occurred within Force.com." : reason;
		break;
	default:
		if (statusCode >= 200 && statusCode < 300) {
			message = reason;
			isError = false;
		} else if (reply->error() != QNetworkReply::NoError) {
			message = reason.isNull() || reason.isEmpty() ? reply->errorString() : reason;
		} else {
			//unhandled status code, we treat it as error
			message = reason;
		}
		break;
	}

	if (isError) {
		mResult = mResult ? mResult : SFResult::createErrorResult(statusCode, message);
		if (needToRetry) {
			return StateNeedToRetry;
		} else {
			return StateError;
		}
	} else {
		mResult = SFResult::create();
		mResult->mStatus = SFResult::TaskResultSuccess;
		mResult->mCode = statusCode;
		mResult->mMessage = message;
		return StateFinished;
	}
}

SFNetworkAccessTask::NetworkTaskState SFRestResourceTask::processNetworkErrorCode(const int & errorCode, const QString & reason) {
	if (errorCode == QNetworkReply::NoError) {
		//no error, shouldn't reach here if we process HTTP status code first
		mResult = SFResult::create();
		mResult->mStatus = SFResult::TaskResultSuccess;
		mResult->mCode = errorCode;
		mResult->mMessage = reason;
		return StateFinished;
	} else {
		mResult = mResult ? mResult : SFResult::createErrorResult(errorCode, reason);
		return StateError;
	}
}

SFNetworkAccessTask::NetworkTaskState SFRestResourceTask::parseJsonContent(const QVariant & jsonContent) {
	//parse payload
	if (!mResult) {
		mResult = SFResult::create();
		mResult->mStatus = SFResult::TaskResultSuccess;
	}
	mResult->mPayload = jsonContent;

	//additional or overwrite message
	if (jsonContent.canConvert<QVariantMap>()) {
		QVariantMap map = jsonContent.value<QVariantMap>();
		if (map.contains("errorCode") && !map["errorCode"].toString().isNull() && !map["errorCode"].toString().isEmpty()) {
			mResult->mMessage += "\n";
			mResult->mMessage += map["errorCode"].toString();
		}

		if (map.contains("message") && !map["message"].toString().isNull() && !map["message"].toString().isEmpty()) {
			mResult->mMessage += "\n";
			mResult->mMessage += map["message"].toString();
		}

		if (map.contains("errors") && map["errors"].toStringList().size() > 0) {
			mResult->mMessage += "\n";
			mResult->mMessage += map["errors"].toStringList().join("\n");
		}

		if (map.contains("fields") && map["fields"].toList().size() > 0) {
			QVariantList fields = map["fields"].toList();
			QStringList fieldNames;
			for(QVariantList::const_iterator i=fields.constBegin(); i!=fields.constEnd(); i++) {
				QString name = i->toString();
				if (!name.trimmed().isEmpty()) {
					fieldNames << name;
				}
			}
			if (!fieldNames.isEmpty()) {
				mResult->mMessage += "\nRelated fields: ";
				mResult->mMessage += fieldNames.join(", ");
			}
		}

		if (map.contains("id") && !map["id"].toString().isNull() && !map["id"].toString().isEmpty()) {
			// we overwrite message there
			mResult->mMessage = map["id"].toString();
		}
	}

	return StateFinished;
}

} /* namespace sf */
