/*
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

class SFIdleScreenTracker : public QObject, public bb::AbstractBpsEventHandler {
	Q_OBJECT

signals:
	void screenIdleTimeout();
public slots:
	void startTracking(const int & timeoutSeconds = 0);
	void stopTracking();
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
