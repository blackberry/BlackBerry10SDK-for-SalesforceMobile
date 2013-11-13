/*
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

class SFRestResourceTask : public sf::SFNetworkAccessTask {
	Q_OBJECT
public slots:

public:
	SFRestResourceTask();
	SFRestResourceTask(QNetworkAccessManager * const networkAccessManager);
	SFRestResourceTask(QNetworkAccessManager * const networkAccessManager, SFRestRequest * request);

	virtual ~SFRestResourceTask();

protected:
	virtual NetworkTaskState ensureRequest();

	/*
	 * Override note: The overridden function shouldn't assume the existence of mResult object, neither
	 * should it assume that the network response contains success status code. The overridden function
	 * should check current mResult object for any server side errors.
	 * However, it can assume that the "jsonContent" is parsed from HTTP response body and is valid
	 * QVariantMap or QVariantList. The HTTP status code and QNetworkReply error code has been processed
	 * when this function is called.
	 */
	virtual NetworkTaskState parseJsonContent(const QVariant & jsonContent);

	NetworkTaskState processReply(QNetworkReply * reply);
	NetworkTaskState processStatusCode(const int & statusCode, const QString & reason, QNetworkReply * reply);
	NetworkTaskState processNetworkErrorCode(const int & errorCode, const QString & reason);

	SFRestRequest *mRestRequest;
};

} /* namespace sf */
Q_DECLARE_METATYPE(sf::SFRestResourceTask*)
#endif /* RESTRESOURCETASK_H_ */
