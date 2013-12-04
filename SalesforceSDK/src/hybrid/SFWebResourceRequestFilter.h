/*
 * SFWebResourceRequestFilter.h
 *
 *  Created on: Dec 2, 2013
 *      Author: timshi
 */

#ifndef SFWEBRESOURCEREQUESTFILTER_H_
#define SFWEBRESOURCEREQUESTFILTER_H_

#include <bb/cascades/WebResourceRequestFilter>
#include <bb/cascades/WebResourceRequest>

namespace sf {
using namespace bb::cascades;
class SFWebResourceRequestFilter : public WebResourceRequestFilter{
public:
	SFWebResourceRequestFilter();
	virtual ~SFWebResourceRequestFilter();

    FilterAction filterResourceRequest(WebResourceRequest* request, RequestPurpose purpose);
    bool filterResourceError(int status, const QUrl& url, const QString& message);
    bool filterResourceOpened(WebResourceRequest* request, int& status, QString& message);
    bool filterResourceHeader(WebResourceRequest* request, QString& key, QString& value);
    bool filterResourceData(WebResourceRequest* request, QByteArray& data);
    bool filterResourceDone(WebResourceRequest* request);
};
} /* namespace sf */
#endif /* SFWEBRESOURCEREQUESTFILTER_H_ */
