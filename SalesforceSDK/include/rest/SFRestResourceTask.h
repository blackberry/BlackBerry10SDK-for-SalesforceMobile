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
