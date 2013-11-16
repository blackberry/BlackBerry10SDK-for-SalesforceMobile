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
* SFRestRequest.h
*
*  Created on: Oct 17, 2013
*      Author: Livan Yi Du
*/

#ifndef SFRESTREQUEST_H_
#define SFRESTREQUEST_H_

#include <QObject>
#include <QVariant>
#include "SFGlobal.h"

class QNetworkRequest;

namespace sf {

extern const QString DefaultEndpoint;

class SFRestRequest : public QObject {
	Q_OBJECT
	Q_ENUMS(HTTPContentType)
	Q_PROPERTY(QString endPoint READ endPoint WRITE setEndPoint)
	Q_PROPERTY(QString apiVersion READ apiVersion WRITE setApiVersion)
	Q_PROPERTY(QString path READ path WRITE setPath)
	Q_PROPERTY(sf::HTTPMethod::Type method READ method WRITE setMethod)
	Q_PROPERTY(QString userAgent READ userAgent WRITE setUserAgent)
	Q_PROPERTY(QVariantMap requestParams READ requestParams WRITE setRequestParams)
	Q_PROPERTY(sf::SFRestRequest::HTTPContentType paramsContentType READ paramsContentType WRITE setParamsContentType)
	Q_PROPERTY(QByteArray requestRawData READ requestRawData WRITE setRequestRawData)
	Q_PROPERTY(QVariantMap requestRawHeaders READ requestRawHeaders WRITE setRequestRawHeaders)
public:
	enum HTTPContentType {
		HTTPContentTypeUrlEncoded,
		HTTPContentTypeJSON,
		HTTPContentTypeMultiPart,
	};
	explicit SFRestRequest(QObject * parent = NULL,
			const QString & path = "",
			const HTTPMethodType & method = HTTPMethod::HTTPGet,
			const QString & apiVersion = "",
			const QString & endPoint = DefaultEndpoint,
			const QString & userAgent = QString());

	virtual ~SFRestRequest();

	friend class SFRestResourceTask;

	SFResultCodeType prepareNetworkRequest(QNetworkRequest *pOutRequest, QByteArray *pOutData, QString *pOutErrorMsg);
	static QString extractAccessToken(const QNetworkRequest & request);

	/* accessors */
	const QString & endPoint() const {return this->mEndPoint;};
	void setEndPoint(const QString & endPoint) {this->mEndPoint = endPoint;};
	const QString & apiVersion() const {return this->mApiVersion;};
	void setApiVersion(const QString & apiVersion) {this->mApiVersion = apiVersion;};
	const QString & path() const {return this->mPath;};
	void setPath(const QString & path) {this->mPath = path;};
	const HTTPMethodType & method() const {return this->mMethod;};
	void setMethod(const HTTPMethodType & method) {this->mMethod = method;};
	const QString & userAgent() const {return this->mUserAgent;};
	void setUserAgent(const QString & userAgent) { this->mUserAgent = userAgent;};
	const QVariantMap & requestParams() const {return this->mRequestParams;};
	void setRequestParams(const QVariantMap & params) {this->mRequestParams = params;};
	const HTTPContentType & paramsContentType() const {return this->mParamsContentType;};
	void setParamsContentType(const HTTPContentType & contentType) {this->mParamsContentType = contentType;};
	const QByteArray & requestRawData() const {return this->mRequestRawData;};
	void setRequestRawData(const QByteArray &rawData) {this->mRequestRawData = rawData;};
	const QVariantMap & requestRawHeaders() const {return this->mRequestRawHeaders;};
	void setRequestRawHeaders(const QVariantMap & rawHeaders ) {this->mRequestRawHeaders = rawHeaders;};

	Q_INVOKABLE QVariant param(const QString & key);
	Q_INVOKABLE void setParam(const QString & key, const QVariant & value);
	Q_INVOKABLE void removeParam(const QString & key);
private:
	QString mPath;
	HTTPMethodType mMethod;
	QString mApiVersion;
	QString mEndPoint;
	QString mUserAgent;
	QVariantMap mRequestParams;
	HTTPContentType mParamsContentType;
	QByteArray mRequestRawData;
	QVariantMap mRequestRawHeaders;

	SFResultCodeType createNetworkRequest(QNetworkRequest *pOutRequest, QString * pOutErrorMsg);
	bool encodeParamsToURL(QUrl & url);
	bool encodeParamsToData(QByteArray * pOutBytes, QString * pOutContentType);
	void createUrlEncodedContent(QByteArray *pOutBytes);

	//for debug
	QString composeRequestSummary(const QNetworkRequest & request, QByteArray & data);
};

} /* namespace sf */

Q_DECLARE_METATYPE(sf::SFRestRequest*)
#endif /* SFRESTREQUEST_H_ */
