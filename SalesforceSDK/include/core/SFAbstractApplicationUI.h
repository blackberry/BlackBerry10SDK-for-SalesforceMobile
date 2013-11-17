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

/*!
 * @class SFAbstractApplicationUI
 * @headerfile SFAbstractApplicationUI.h "SFAbstractApplicationUI.h"
 * @brief Abstract base class for the main application UI class
 *
 * @details
 * This abstract class's constructor contains the minimum plumbing required to setting up
 * the application to be able to start using the authentication and REST APIs included
 * in this SDK
 *
 * Usage
 * -----
 * Create a concrete subclass of this class.
 * @code{.cpp}
 *   ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
 *   	sf::SFAbstractApplicationUI(app)
 *   	{
 *   		//application specific setup
 *   	}
 * @endcode
 *
 * \sa SFAuthenticationManager, SFRestAPI
 *
 * \author Livan Yi Du
 */
class SFAbstractApplicationUI : public QObject {
	Q_OBJECT
public:

	SFAbstractApplicationUI(bb::cascades::Application *app);
	virtual ~SFAbstractApplicationUI();

};

} /* namespace sf */
#endif /* SFABSTRACTAPPLICATIONUI_H_ */
