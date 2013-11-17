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
* SFIdleScreenTracker.cpp
*
*  Created on: Nov 5, 2013
*      Author: Livan Yi Du
*/

#include "SFIdleScreenTracker.h"
#include <QTimer>
#include <SFGlobal.h>

namespace sf {

static const int DefaultTimeoutSeconds = 600;

using namespace bb;
SFIdleScreenTracker::SFIdleScreenTracker(QObject * parent)
: QObject(parent), AbstractBpsEventHandler(), mActive(false), mTimeoutSeconds(DefaultTimeoutSeconds), mIdleNotificationTimer() {

	//configure screen context
	mScreenContext = NULL;
	screen_create_context(&mScreenContext, SCREEN_APPLICATION_CONTEXT);

	// Initialize and subscribe the BPS
	bps_initialize();
	subscribe(screen_get_domain());

	//initialize timer
	mIdleNotificationTimer = new QTimer(this);
	mIdleNotificationTimer->setSingleShot(true);
	connect(mIdleNotificationTimer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
}

SFIdleScreenTracker::~SFIdleScreenTracker() {
	// Close the BPS library
	bps_shutdown();
	screen_destroy_context(mScreenContext);
}

/***************************
 * Overrides & slots
 ***************************/
void SFIdleScreenTracker::event(bps_event_t *event)
{
    // Check events
    if (bps_event_get_domain(event) != screen_get_domain()) {
    	return;
    }
    screen_event_t screen_event = screen_event_get_event(event);
    int eventType;
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &eventType);
    if (eventType != SCREEN_EVENT_IDLE) {
    	return;
    }

    int state;
    if (screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_IDLE_STATE, &state) != 0) {
    	//can't get state, will bail
    	sfWarning() << "[SFIdleScreenTracker] Cannot read idle state from BPS event.";
    }
    if (state == 0) {
    	//exitting idle state
    	mIdleNotificationTimer->stop();
    } else {
    	//find out system's idle timeout
    	screen_display_t display = NULL;
    	screen_get_event_property_pv(screen_event, SCREEN_PROPERTY_DISPLAY, reinterpret_cast<void**>(&display));
    	long long timeout = 0;
    	if (screen_get_display_property_llv(display, SCREEN_PROPERTY_IDLE_TIMEOUT, &timeout) == 0 && timeout != 0) {
    		int msecRemaining = (mTimeoutSeconds * 1000) - (timeout / 1000000);
    		mIdleNotificationTimer->start(msecRemaining);
    	}
    }
}

/***************************
 * APIs & public slots
 ***************************/
void SFIdleScreenTracker::onTimerTimeout() {
	if (!mActive) {
		//not active silently bail
		return;
	}
	if (currentScreenIdleState() != ScreenIdle) {
		sfWarning() << "[SFIdleScreenTracker] Screen idle timer is fired when the screen is not idle. Will bail";
		return;
	}
	sfWarning() << "[SFIdleScreenTracker] Screens have been idle for" << mTimeoutSeconds << "seconds.";
	emit screenIdleTimeout();
}

void SFIdleScreenTracker::startTracking(const int & timeoutSeconds) {
	if (mActive) {
		return;
	}
	if (screen_request_events(mScreenContext) != BPS_SUCCESS) {
		return;
	}

	//reset timer
	if (timeoutSeconds > 0) {
		mTimeoutSeconds = timeoutSeconds;
	}
	if (currentScreenIdleState() == ScreenIdle) {
		//it's already in idle state, we start the timer now
		mIdleNotificationTimer->start(mTimeoutSeconds * 1000);
	}
	mActive = true;
	return;
}

void SFIdleScreenTracker::stopTracking() {
	if (!mActive) {
		return;
	}

	mActive = false;
	mIdleNotificationTimer->stop();
	screen_stop_events(mScreenContext);
	return;
}

void SFIdleScreenTracker::setTimeoutSeconds(const int & timeoutSeconds) {
	if (timeoutSeconds <= 0) {
		return;
	}
	mTimeoutSeconds = timeoutSeconds;
	if (mIdleNotificationTimer->isActive()) {
		mIdleNotificationTimer->start(mTimeoutSeconds * 1000);
	}
}

/***************************
 * Protected
 ***************************/
SFIdleScreenTracker::ScreenIdleState SFIdleScreenTracker::currentScreenIdleState() {
	int count;
	screen_get_context_property_iv(mScreenContext, SCREEN_PROPERTY_DISPLAY_COUNT, &count);
	screen_display_t displays[count];
	screen_get_context_property_pv(mScreenContext, SCREEN_PROPERTY_DISPLAYS, reinterpret_cast<void**>(&displays));

	//if any display is in idle state, return idle. If there is no display (possible?), return non-idle
	int idle;
	for(int i=0; i<count; i++) {
		if (displays[i] == NULL || screen_get_display_property_iv(displays[i], SCREEN_PROPERTY_IDLE_STATE, &idle) != 0) {
			sfWarning() << "[SFIdleScreenTracker] Cannot read idle state from display:" << displays[i];
			continue; //can't ready idle state, ignore it
		}

		if (idle == 1) {
			return ScreenIdle;
		}
	}
	return ScreenNotIdle;
}

} /* namespace sf */
