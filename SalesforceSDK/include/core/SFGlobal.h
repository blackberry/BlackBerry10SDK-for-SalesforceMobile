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
* Constants.h
*
*  Created on: Oct 18, 2013
*      Author: Livan Yi Du
*/

#ifndef SFCONSTANTS_H_
#define SFCONSTANTS_H_

#include <QObject>
#include <QDebug>

class QNetworkAccessManager;

/*! @namespace sf
 * @brief The namespace of Blackberry 10 SDK for Salesforce */
namespace sf {

extern const QString SFMobileSDKVersion; //!< Version string of the current SDK
extern const QString SFMobileSDKNativeDesignator; //!< "Native"
extern const QString kSFRestRequestTag; //!< The key used to retrieve tag object carried in @c SFResult
extern const QString kSFOAuthError; //!< The key for oAuth error in response
extern const QString kSFOAuthErrorDescription; //!< The key for oAuth error description in response

/*
 * Macros for logging
 * We don't use QT_NO_DEBUG_OUTPUT or QT_NO_WARNING_OUTPUT because we want the SDK to have
 * separated log level from the application.
 * Use SF_NO_DEBUG_OUTPUT and SF_NO_WARNING_OUTPUT to control SDK logging level.
 *
 * Note 1: QT macros overrides SF logging macros. i.e. if you have QT_NO_WARNING_OUTPUT defined,
 * 			There will be no warning logs no matter if you defined SF_NO_WARNING_OUTPUT.
 * Note 2: SF_NO_WARNING_OUTPUT implies SF_NO_DEBUG_OUTPUT
 *
 * e.g
 * #define SF_NO_DEBUG_OUTPUT
 * #define SF_NO_WARNING_OUTPUT
 */

#if defined(SF_NO_WARNING_OUTPUT)
inline QNoDebug sfDebug() { return QNoDebug(); }
inline QNoDebug sfWarning() { return QNoDebug(); }
#elif defined(SF_NO_DEBUG_OUTPUT)
inline QNoDebug sfDebug() { return QNoDebug(); }
inline QDebug sfWarning() { return qWarning(); }
#else
inline QDebug sfDebug() { return qDebug(); }
inline QDebug sfWarning() { return qWarning(); }
#endif


/*
 * Global Class
 * In order to expose enums and constent values to QML, we need wrap them
 * into QObject with Q_OBJECT macro
 */
/*! @headerfile SFGlobal.h <core/SFGlobal.h>
 * @brief Supported HTTP verbs @see SFRestRequest, SFRestResourceTask */
class HTTPMethod : public QObject {
	Q_OBJECT
	Q_ENUMS(Type)
public:
	enum Type {
		HTTPGet = 0, //!< HTTP GET
		HTTPHead = 1, //!< HTTP HEAD
		HTTPDelete = 2, //!< HTTP DELETE
		HTTPPost = 3, //!< HTTP POST
		HTTPPut = 4, //!< HTTP PUT
		HTTPPatch = 5 //!< HTTP PATCH
	};
};
typedef enum HTTPMethod::Type HTTPMethodType; //!< @c HTTPMethod::Type

/*! @headerfile SFGlobal.h <core/SFGlobal.h>
 * @brief Pre-defined SDK error codes @see SFResult,
 * <a href="http://www.salesforce.com/us/developer/docs/api_rest/Content/errorcodes.htm">Salesforce REST API status code</a> */
class SFResultCode : public QObject {
	Q_OBJECT
	Q_ENUMS(Type)
public:
	enum Type {
		SFErrorNoError = 0, //!< Success, no error.
		SFErrorGeneric = -1, //!< Generic/Unknown error.
		SFErrorCancelled = -2, //!< The task is canceled.
		SFErrorNetwork = -3, //!< Generic network error.
		SFErrorInvalidAccessToken = -4, //!< The Force.com access token is not available.

		SFMinimumRestStatus = 200,
		SFRestStatusSuccess = 200, //!< HTTP 200. Success
		SFRestStatusSuccessCreated = 201, //!< HTTP 201. Created
		SFRestStatusSuccessNoContent = 204, //!< HTTP 204. Success, and response body is empty.
		SFRestStatusMultiRecords = 300, //!< HTTP 300. Found multiple records match the given criteria.
		SFRestStatusNoChanges = 304, //!< HTTP 304. No changes since the given date and time.
		SFRestStatusBadData = 400, //!< HTTP 400. Bad request. The request cannot be understood.
		SFRestStatusInvalidSession = 401, //!< HTTP 401. The session has been expired.
		SFRestStatusRefused = 403, //!< HTTP 403. The request has been refused, possibly due to insufficient permission.
		SFRestStatusNotFound = 404, //!< HTTP 404. The requested resource is not found.
		SFRestStatusMethodNotAllowed = 405, //!< HTTP 405. The given action/verb is not allowed for the resource.
		SFRestStatusFormatNotSupported = 415, //!< HTTP 415. The requested data format is not supported.
		SFRestStatusServerError = 500, //!< HTTP 500. Error at server side.
	};
};
typedef enum SFResultCode::Type SFResultCodeType; //!< @c SFResultCode::Type

/*
 * Global Function
 */
/*! Register necessary meta types for the SDK to be accessible from QML */
void sfRegisterMetaTypes();

/*! Get a shared instance of @c QNetworkAccessManager. The object is created and configured when the first time being called. */
QNetworkAccessManager* getSharedNetworkAccessManager();

} /* namespace rest */

#endif /* SFCONSTANTS_H_ */
