/*
 * SFAbstractApplicationUI.h
 *
 *  Created on: Nov 7, 2013
 *      Author: Livan Yi Du
 */

#ifndef SFABSTRACTAPPLICATIONUI_H_
#define SFABSTRACTAPPLICATIONUI_H_

#include <QObject>
#include <bb/cascades/Application>
#include "SFGlobal.h"

namespace sf {

/*
 * Subclass should define following constants as basic application setup. Otherwise you will receive complier error
 */
extern QString const SFRemoteAccessConsumerKey;
extern QString const SFOAuthRedirectURI;
extern QString const SFDefaultRestApiVersion;

class SFAbstractApplicationUI : public QObject {
	Q_OBJECT
public:

	SFAbstractApplicationUI(bb::cascades::Application *app);
	virtual ~SFAbstractApplicationUI();

};

} /* namespace sf */
#endif /* SFABSTRACTAPPLICATIONUI_H_ */
