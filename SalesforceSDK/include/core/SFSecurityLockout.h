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

/*!
 * @class SFSecurityLockout
 * @headerfile SFSecurityLockout.h <core/SFSecurityLockout.h>
 *
 * @brief @c SFSecurityLockout manages the process involved with locking the device when user is idle
 * The class is meant to be used by the classes in the SDK.
 */
class SFSecurityLockout : public QObject {
	Q_OBJECT

private:
	bb::cascades::Sheet* mCreatePasscodeSheet;
	bb::cascades::Sheet* mLockScreenSheet;
	SFIdleScreenTracker* mIdleScreenTracker;
public:
	static SFSecurityLockout* instance();
	virtual ~SFSecurityLockout();

	/*!
	 * presents the passcode creation screen
	 */
	void createPasscode();
	/*!
	 * locks the app with the enter password screen
	 */
	void lock();
	/*!
	 * cancels the lock
	 */
	void cancelLock();
	/*!
	 * @return if the user has set a passcode for the device
	 */
	bool hasPasscode();
	/*!
	 * @return if is currently in the process of creating passcode
	 */
	bool isCreatingPasscode();

	/*!
	 * logs out and resets everything
	 */
	Q_INVOKABLE void reset();
	/*!
	 * save passcode and starts the monitoring for user activity
	 */
	Q_INVOKABLE void savePasscode(QString passcode);
	/*!
	 * @return if passcode matches returns true and unlocks the screen and starts the monitoring.
	 * otherwise return false.
	 */
	Q_INVOKABLE bool validatePasscode(QString passcode);
	/*!
	 * @return the required passcode length
	 */
	Q_INVOKABLE int requiredPasscodeLength();
	/*!
	 * @return the activity tracker
	 */
	SFIdleScreenTracker* getIdleScreenTracker() const;
	/*!
	 * Do the clean up required on app exit
	 */
	void onAboutToQuit();
	/*!
	 * @return the Sheet for the create passcode screen
	 */
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
