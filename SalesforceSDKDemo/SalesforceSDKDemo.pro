APP_NAME = SalesforceSDKDemo

CONFIG += qt warn_on cascades10

include(config.pri)

PRE_TARGETDEPS ~= s/.*SalesforceSDK.*/ #remove incorrect target

CONFIG += salesforcesdk

LIBS += -lbbdata


