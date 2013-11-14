/*
 * SFSecurityLockout.h
 *
 *  Created on: Nov 1, 2013
 *      Author: timshi
 */

#ifndef SFSECURITYLOCKOUT_H_
#define SFSECURITYLOCKOUT_H_

#include <QObject>
#include <bb/cascades/Sheet>

namespace sf {

class SFIdleScreenTracker;

class SFSecurityLockout : public QObject {
	Q_OBJECT

private:
	bb::cascades::Sheet* mCreatePasscodeSheet;
	bb::cascades::Sheet* mLockScreenSheet;
	SFIdleScreenTracker* mIdleScreenTracker;
public:
	static SFSecurityLockout* instance();
	virtual ~SFSecurityLockout();

	void createPasscode(); //presents the passcode creation screen
	void lock();
	void cancelLock();
	bool hasPasscode();
	bool isCreatingPasscode();

	Q_INVOKABLE void reset(); //logs out and resets everything
	Q_INVOKABLE void savePasscode(QString passcode); //save passcode and starts the monitoring
	Q_INVOKABLE bool validatePasscode(QString passcode); //unlocks the screen and starts the monitoring
	Q_INVOKABLE int requiredPasscodeLength();

	SFIdleScreenTracker* getIdleScreenTracker() const;

	void onAboutToQuit();
	bb::cascades::Sheet* getCreatePasscodeSheet();
signals:
	void devicePasscodeCreated();
protected:
	static SFSecurityLockout * sharedInstance;

	SFSecurityLockout();
	void setCreatePasscodeSheet(bb::cascades::Sheet* sheet);
	void setLockScreenSheet(bb::cascades::Sheet* sheet);
	void setIdleScreenTracker(SFIdleScreenTracker* idleScreenTracker);

private slots:
	/*
	 * this is called when activity monitor's timer fires
	 * when this happens, call lock screen function
	 */
	void onLockScreenTimerExpired();
	void unlock();
};
} /* namespace sf */
#endif /* SFSECURITYLOCKOUT_H_ */
