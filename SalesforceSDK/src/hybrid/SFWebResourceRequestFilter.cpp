/*
 * SFWebResourceRequestFilter.cpp
 *
 *  Created on: Dec 2, 2013
 *      Author: timshi
 */

#include "SFWebResourceRequestFilter.h"

namespace sf {

using namespace bb::cascades;

SFWebResourceRequestFilter::SFWebResourceRequestFilter() {
	// TODO Auto-generated constructor stub

}

SFWebResourceRequestFilter::~SFWebResourceRequestFilter() {
	// TODO Auto-generated destructor stub
}

WebResourceRequestFilter::FilterAction SFWebResourceRequestFilter::filterResourceRequest(WebResourceRequest* request, RequestPurpose purpose){
	return Filter;
}
bool SFWebResourceRequestFilter::filterResourceError(int status, const QUrl& url, const QString& message){
	return false;
}
bool SFWebResourceRequestFilter::filterResourceOpened(WebResourceRequest* request, int& status, QString& message){
	return false;
}
bool SFWebResourceRequestFilter::filterResourceHeader(WebResourceRequest* request, QString& key, QString& value){
	qDebug()<<"filter header key " << key << " value" << value;
	if (key == "X-XSS-Protection"){
		value = "0";
	}
	return false;
}
bool SFWebResourceRequestFilter::filterResourceData(WebResourceRequest* request, QByteArray& data){
	return false;
}
bool SFWebResourceRequestFilter::filterResourceDone(WebResourceRequest* request){
	return false;
}

} /* namespace sf */
