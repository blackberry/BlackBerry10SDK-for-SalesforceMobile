/*
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

namespace sf {

extern const QString SFMobileSDKVersion;
extern const QString SFMobileSDKNativeDesignator;
extern const QString kSFRestRequestTag;
extern const QString kSFOAuthError;
extern const QString kSFOAuthErrorDescription;

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
class HTTPMethod : public QObject {
	Q_OBJECT
	Q_ENUMS(Type)
public:
	enum Type {
		HTTPGet = 0,
		HTTPHead = 1,
		HTTPDelete = 2,
		HTTPPost = 3,
		HTTPPut = 4,
		HTTPPatch = 5
	};
};
typedef enum HTTPMethod::Type HTTPMethodType;

class SFResultCode : public QObject {
	Q_OBJECT
	Q_ENUMS(Type)
public:
	enum Type {
		SFErrorNoError = 0,
		SFErrorGeneric = -1,
		SFErrorCancelled = -2,
		SFErrorNetwork = -3,
		SFErrorInvalidAccessToken = -4,

		SFMinimumRestStatus = 200,
		SFRestStatusSuccess = 200,
		SFRestStatusSuccessCreated = 201,
		SFRestStatusSuccessNoContent = 204,
		SFRestStatusMultiRecords = 300,
		SFRestStatusNoChanges = 304,
		SFRestStatusBadData = 400,
		SFRestStatusInvalidSession = 401,
		SFRestStatusRefused = 403,
		SFRestStatusNotFound = 404,
		SFRestStatusMethodNotAllowed = 405,
		SFRestStatusFormatNotSupported = 415,
		SFRestStatusServerError = 500,
	};
};
typedef enum SFResultCode::Type SFResultCodeType;

/*
 * Global Function
 */
void sfRegisterMetaTypes();
QNetworkAccessManager* getSharedNetworkAccessManager();

} /* namespace rest */

#endif /* SFCONSTANTS_H_ */
