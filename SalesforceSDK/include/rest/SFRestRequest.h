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

extern const QString DefaultEndpoint; //!< Default Force.com REST API end point: "/services/data"

/*!
 * @class SFRestRequest
 * @headerfile SFRestRequest.h <rest/SFRestRequest.h>
 * @brief The SFRestRequest holds a request that comply with Force.com REST API standard and can be sent using @c SFRestAPI.
 *
 * @details
 * The class is responsible to hold necessary information and generate a @c QNetworkRequest that is acceptable by Force.com REST API. If request parameters are added,
 * you need to choose the encoding method by setting property @c SFRestRequest::paramsContentType. For regular usage, you don't need
 * to construct the object by yourself. @c SFRestAPI provide with convenient functions for you to create various SFRestRequest objects.
 * This class also support customized request body if built-in parameter encoding is not sufficient.
 *
 * For security reason, this class do not hold information about current Salesforce session. It retrieves access token and instance URL at runtime.
 * This means if you logout after you create a request, the class cannot inject valid access token into generated @c QNetworkRequest.
 *
 * \sa SFRestAPI, SFRestResourceTask
 *
 * \author Livan Yi Du
 */
class SFRestRequest : public QObject {
	Q_OBJECT
	Q_ENUMS(HTTPContentType)
	Q_PROPERTY(QString endPoint READ endPoint WRITE setEndPoint) /*!< The end point of this request, relative to the session's instance URL. @b Default: "/services/data" */
	Q_PROPERTY(QString apiVersion READ apiVersion WRITE setApiVersion) /*!< The Force.com REST API version of this request. e.g. "/29.0" */

	/*! The relative/absolute path of requested resource. If the value is relative path, the path will be appended after Force.com
	 * session's instance URL, @c SFRestRequest::endPoint and @c SFRestRequest::apiVersion. If absolute path is given,
	 * @c SFRestRequest::endPoint and @c SFRestRequest::apiVersion are ignored*/
	Q_PROPERTY(QString path READ path WRITE setPath)
	Q_PROPERTY(sf::HTTPMethod::Type method READ method WRITE setMethod) /*!< The HTTP verb of this request. For possible values, see @c HTTPMethod::Type */
	Q_PROPERTY(QString userAgent READ userAgent WRITE setUserAgent) /*!< The user agent of this request. The default value is set by the SDK */

	/*! A collection of key-value pairs contains parameters that you want to send in this request. The contained values is converted into
	 * @c QString when the request is used. Any value that cannot be converted to QString will be ignored. */
	Q_PROPERTY(QVariantMap requestParams READ requestParams WRITE setRequestParams)

	/*!< The encoding type of parameters. @note This property is ignored and @c SFRestRequest::HTTPContentTypeUrlEncoded is
	 * used if HTTP verb is set to GET, HEAD, DELETE. For supported format, see @c SFRestRequest::HTTPContentType */
	Q_PROPERTY(sf::SFRestRequest::HTTPContentType paramsContentType READ paramsContentType WRITE setParamsContentType)
	Q_PROPERTY(QByteArray requestRawData READ requestRawData WRITE setRequestRawData) /*!< The raw data of the request body. @note Assigning this property automatically clear the parameters assigned via @c setRequestParams() */
	Q_PROPERTY(QVariantMap requestRawHeaders READ requestRawHeaders WRITE setRequestRawHeaders) /*!< Key-value pairs that will be added to the request header. @note entries added will overwrite default values. */
public:
	/*! The encoding type of parameters. Typically with Force.com APIs, use @c SFRestRequest::HTTPContentTypeUrlEncoded for GET, HEAD and DELETE
	 * and use @c SFRestRequest::HTTPContentTypeJSON for other HTTP verbs. */
	enum HTTPContentType {
		HTTPContentTypeUrlEncoded, /*!< parameters are encoded into the URL as key-value pair. e.g. "?firstname=John&lastname=Smith" */
		HTTPContentTypeJSON, /*!< parameters are encoded into request body using JSON format. */
		HTTPContentTypeMultiPart, /*!< use HTTP "multipart/form-data" encoding. @warning currently not implemented. */
	};

	/*! Explicit constructor.
	 * @param parent parent QObject.
	 * @param path relative or absolute path of the requested resource. See @c SFRestRequest::path for more details.
	 * @param method HTTP verb of the request. See @c HTTPMethod::Type for possible values.
	 * @param apiVersion Force.com REST API version string. See @c SFRestRequest::apiVersion for more details.
	 * @param endPoint Force.com REST API end point. See @c SFRestRequest::endPoint for more details.
	 * @param userAgent User-Agent field in request's header. If not set, the SDK will use auto-generated string. */
	explicit SFRestRequest(QObject * parent = NULL,
			const QString & path = "",
			const HTTPMethodType & method = HTTPMethod::HTTPGet,
			const QString & apiVersion = "",
			const QString & endPoint = DefaultEndpoint,
			const QString & userAgent = QString());

	virtual ~SFRestRequest(); /*!< Default destructor */

	friend class SFRestResourceTask;

	/*! A helper function that extract the Force.com's access token from given @c QNetworkRequest
	 * @param request the QNetworkRequest
	 * @return the access token used in the given request. */
	static QString extractAccessToken(const QNetworkRequest & request);

	/*! This method prepares all necessary objects that can be used by @c QNetworkAccessManager to send the request.
	 * It populate the @c QNetworkRequest object using Force.com session's instance URL and injects necessary headers for authentication
	 * and data encoding. It also populates a @c QByteArray as request's content body.
	 * @param[out] pOutRequest the pointer to the @c QNetworkRequest to be populated.
	 * @param[out] pOutData the pointer to the @c QByteArray to be filled
	 * @param[out] pOutErrorMsg the pointer to the string buffer for any potential error message.
	 * @return standard @c SFResultCode indicating the result of this operation. See @c SFResultCode::Type for possible values. */
	SFResultCodeType prepareNetworkRequest(QNetworkRequest *pOutRequest, QByteArray *pOutData, QString *pOutErrorMsg);

	/* accessors */
	/*! See @c SFRestRequest::endPoint */
	const QString & endPoint() const {return this->mEndPoint;};
	/*! See @c SFRestRequest::endPoint */
	void setEndPoint(const QString & endPoint) {this->mEndPoint = endPoint;};
	/*! See @c SFRestRequest::apiVersion */
	const QString & apiVersion() const {return this->mApiVersion;};
	/*! See @c SFRestRequest::apiVersion */
	void setApiVersion(const QString & apiVersion) {this->mApiVersion = apiVersion;};
	/*! See @c SFRestRequest::path */
	const QString & path() const {return this->mPath;};
	/*! See @c SFRestRequest::path */
	void setPath(const QString & path) {this->mPath = path;};
	/*! See @c SFRestRequest::method */
	const HTTPMethodType & method() const {return this->mMethod;};
	/*! See @c SFRestRequest::method */
	void setMethod(const HTTPMethodType & method) {this->mMethod = method;};
	/*! See @c SFRestRequest::userAgent */
	const QString & userAgent() const {return this->mUserAgent;};
	/*! See @c SFRestRequest::userAgent */
	void setUserAgent(const QString & userAgent) { this->mUserAgent = userAgent;};
	/*! See @c SFRestRequest::requestParams */
	const QVariantMap & requestParams() const {return this->mRequestParams;};
	/*! See @c SFRestRequest::requestParams */
	void setRequestParams(const QVariantMap & params) {this->mRequestParams = params;};
	/*! See @c SFRestRequest::paramsContentType */
	const HTTPContentType & paramsContentType() const {return this->mParamsContentType;};
	/*! See @c SFRestRequest::paramsContentType */
	void setParamsContentType(const HTTPContentType & contentType) {this->mParamsContentType = contentType;};
	/*! See @c SFRestRequest::requestRawData */
	const QByteArray & requestRawData() const {return this->mRequestRawData;};
	/*! See @c SFRestRequest::requestRawData */
	void setRequestRawData(const QByteArray &rawData) {this->mRequestRawData = rawData;};
	/*! See @c SFRestRequest::requestRawHeaders */
	const QVariantMap & requestRawHeaders() const {return this->mRequestRawHeaders;};
	/*! See @c SFRestRequest::requestRawHeaders */
	void setRequestRawHeaders(const QVariantMap & rawHeaders ) {this->mRequestRawHeaders = rawHeaders;};

	/*! Get the value of the parameter associated with given key
	 * @param key the key of the parameter to get
	 * @return the parameter value encapsulated in @c QVariant.
	 * @sa SFRestRequest::requestParams, setParam(), removeParam() */
	Q_INVOKABLE QVariant param(const QString & key);
	/*! Set the value of the parameter associated with given key
	 * @param key the key of the parameter to set
	 * @param value the @c QVariant value to set to.
	 * @sa SFRestRequest::requestParams, param(), removeParam() */
	Q_INVOKABLE void setParam(const QString & key, const QVariant & value);
	/*! Remove the key-value pair specified by given key
	 * @param key the key of the parameter to remove
	 * @sa SFRestRequest::requestParams, param(), setParam() */
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
