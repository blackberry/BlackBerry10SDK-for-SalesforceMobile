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
* SFSecurityLockout.cpp
*
*  Created on: Nov 1, 2013
*      Author: timshi
*/

#include "SFSecurityLockout.h"
#include <bb/cascades/QmlDocument>
#include "SFAuthenticationManager.h"
#include "SFAccountManager.h"
#include "SFSecurityManager.h"
#include "SFIdleScreenTracker.h"
#include "SFIdentityCoordinator.h"
#include "SFIdentityData.h"

namespace sf {

using namespace bb::cascades;

SFSecurityLockout* SFSecurityLockout::sharedInstance = NULL;

SFSecurityLockout::SFSecurityLockout() {
	mCreatePasscodeSheet = NULL;
	mLockScreenSheet = NULL;
	mIdleScreenTracker = NULL;
}

SFSecurityLockout::~SFSecurityLockout() {
	if (mCreatePasscodeSheet!=NULL){
		mCreatePasscodeSheet->deleteLater();
		mCreatePasscodeSheet = NULL;
	}
	if (mLockScreenSheet!=NULL){
		mLockScreenSheet->deleteLater();
		mLockScreenSheet = NULL;
	}
	if (mIdleScreenTracker!=NULL){
		mIdleScreenTracker->deleteLater();
		mIdleScreenTracker=NULL;
	}
}

SFSecurityLockout* SFSecurityLockout::instance(){
	if (!SFSecurityLockout::sharedInstance){
		sharedInstance = new SFSecurityLockout();

		QmlDocument *qml = QmlDocument::create("asset:///salesforce/SFPasscodeCreation.qml").parent(sharedInstance);
		qml->setContextProperty("SFSecurityLockout", sharedInstance);
		Sheet* sheet = qml->createRootObject<Sheet>();
		sharedInstance->setCreatePasscodeSheet(sheet);

		qml = QmlDocument::create("asset:///salesforce/SFLockScreen.qml").parent(sharedInstance);
		qml->setContextProperty("SFSecurityLockout", sharedInstance);
		qml->setContextProperty("SFAuthenticationManager", SFAuthenticationManager::instance());
		sheet = qml->createRootObject<Sheet>();
		sharedInstance->setLockScreenSheet(sheet);

		SFIdleScreenTracker* monitor = new SFIdleScreenTracker;
		connect(monitor, SIGNAL(screenIdleTimeout()), sharedInstance, SLOT(onLockScreenTimerExpired()), Qt::UniqueConnection);
		sharedInstance->setIdleScreenTracker(monitor);
	}
	return sharedInstance;
}

void SFSecurityLockout::createPasscode(){
	if (SFAccountManager::instance()->getIdCoordinator()->getIdData()->getMobileAppPinLength().toInt()>0
		&&	SFAccountManager::instance()->getIdCoordinator()->getIdData()->getMobileAppScreenLockTimeout().toInt()>0){
		mCreatePasscodeSheet->open();
	}
}

void SFSecurityLockout::lock(){
	if (SFAccountManager::instance()->getIdCoordinator()->getIdData()->getMobileAppPinLength().toInt()>0
		&&	SFAccountManager::instance()->getIdCoordinator()->getIdData()->getMobileAppScreenLockTimeout().toInt()>0){
		mIdleScreenTracker->stopTracking();
		mLockScreenSheet->open();
	}
}
void SFSecurityLockout::cancelLock(){
	mIdleScreenTracker->stopTracking();
	if (mLockScreenSheet->isOpened()){
		mLockScreenSheet->close();
	}
	if (mCreatePasscodeSheet->isOpened()){
		mCreatePasscodeSheet->close();
	}
}

bool SFSecurityLockout::hasPasscode(){
	QSettings setting;
	return setting.contains(SFAccountManager::instance()->devicePasscodeKey());
}

bool SFSecurityLockout::isCreatingPasscode()
{
	return mCreatePasscodeSheet->isOpened();
}


void SFSecurityLockout::unlock(){
	if (mLockScreenSheet->isOpened()){
		mLockScreenSheet->close();
	}
	mIdleScreenTracker->startTracking(
			SFAccountManager::instance()->getIdCoordinator()->getIdData()->getMobileAppScreenLockTimeout().toInt() * 60);
}

void SFSecurityLockout::reset(){
	QSettings setting;
	setting.remove(SFAccountManager::instance()->devicePasscodeKey());
	cancelLock();
}

void SFSecurityLockout::savePasscode(QString passcode){
	QString passcodeHash = SFSecurityManager::instance()->hash(passcode);
	QSettings setting;
	setting.setValue(SFAccountManager::instance()->devicePasscodeKey(), passcodeHash);
	if (mCreatePasscodeSheet!=NULL && mCreatePasscodeSheet->isOpened()){
		mCreatePasscodeSheet->close();
	}
	mIdleScreenTracker->startTracking(SFAccountManager::instance()->getIdCoordinator()->getIdData()->getMobileAppScreenLockTimeout().toInt() * 60);
	emit devicePasscodeCreated();
}

bool SFSecurityLockout::validatePasscode(QString passcode){
	QString passcodeHash = SFSecurityManager::instance()->hash(passcode);
	QSettings setting;
	QString savedPasscodeHash = setting.value(SFAccountManager::instance()->devicePasscodeKey()).toString();
	if (passcodeHash==savedPasscodeHash){
		unlock();
		return true;
	}
	return false;
}

int SFSecurityLockout::requiredPasscodeLength(){
	return SFAccountManager::instance()->getIdCoordinator()->getIdData()->getMobileAppPinLength().toInt();
}

SFIdleScreenTracker* SFSecurityLockout::getIdleScreenTracker() const{
	return mIdleScreenTracker;
}
void SFSecurityLockout::onAboutToQuit(){
	if (mCreatePasscodeSheet){
		mCreatePasscodeSheet->deleteLater();
	}
	if (mLockScreenSheet){
		mLockScreenSheet->deleteLater();
	}
}
Sheet* SFSecurityLockout::getCreatePasscodeSheet(){
	return mCreatePasscodeSheet;
}
void SFSecurityLockout::onLockScreenTimerExpired(){
	lock();
}

void SFSecurityLockout::setCreatePasscodeSheet(Sheet* sheet){
	mCreatePasscodeSheet = sheet;
}

void SFSecurityLockout::setLockScreenSheet(bb::cascades::Sheet* sheet){
	mLockScreenSheet = sheet;
}

void SFSecurityLockout::setIdleScreenTracker(SFIdleScreenTracker* idleScreenTracker){
	mIdleScreenTracker = idleScreenTracker;
}

} /* namespace sf */
