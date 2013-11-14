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

/*! \mainpage SalesforceBB10SDK
 *
 * \section intro_sec Introduction
 * The main APIs in the SalesforceSDK are found in the SFAuthenticationManager class and SFRestAPI class.
 * The SFAuthenticationManager class provides APIs for the application to manage the oAuth 2.0 authentication process.
 * The SFRestAPI class provides a set of interfaces to work with force.com REST APIs. The SFRestAPI handles the necessary step required to send a request (such as refreshing an access token when its expired).
 *
 * \section install_sec Setup
 */

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
