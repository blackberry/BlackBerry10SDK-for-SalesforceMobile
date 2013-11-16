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
* SFIdleScreenTracker.h
*
* required library:
* LIBS += -lbbdata
*
*  Created on: Nov 5, 2013
*      Author: Livan Yi Du
*/

#ifndef SFIDLESCREENTRACKER_H_
#define SFIDLESCREENTRACKER_H_

#include <QObject>
#include <bb/AbstractBpsEventHandler>
#include <bps/bps.h>
#include <bps/screen.h>

class QTimer;

namespace sf {
using namespace bb;

/*!
 * @class SFIdleScreenTracker
 * @headerfile SFIdleScreenTracker.h <core/SFIdleScreenTracker.h>
 *
 * @brief @c SFIdleScreenTracker monitors user activity and emits a singal when the user has been idle for the
 * specified amount of time. This class is meant to be used by the SDK classes.
 */
class SFIdleScreenTracker : public QObject, public bb::AbstractBpsEventHandler {
	Q_OBJECT

signals:
	/*!
	 * Singal emitted when the screen idle timer times out.
	 */
	void screenIdleTimeout();
public slots:
	/*!
	 * starts tracking
	 */
	void startTracking(const int & timeoutSeconds = 0);
	/*!
	 * stops tracking
	 */
	void stopTracking();
	/*!
	 * handles the screen idle signal from BPS service.
	 */
	void onTimerTimeout();
public:
	enum ScreenIdleState {
		ScreenNotIdle = 0,
		ScreenIdle = 1
	};
	SFIdleScreenTracker(QObject * parent = NULL);
	virtual ~SFIdleScreenTracker();

	virtual void event(bps_event_t *event);

	/* accessors */
	bool isActive() const {return mActive;};
	int timeoutSeconds() const {return mTimeoutSeconds;};
	void setTimeoutSeconds(const int & timeoutSeconds);

protected:
	bool mActive;
	int mTimeoutSeconds;
	QTimer *mIdleNotificationTimer;
	screen_context_t mScreenContext;

	ScreenIdleState currentScreenIdleState();
};

} /* namespace sf */
#endif /* SFIDLESCREENTRACKER_H_ */
