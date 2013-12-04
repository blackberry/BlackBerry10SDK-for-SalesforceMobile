APP_NAME = SalesforceSDKHybridDemo

CONFIG += qt warn_on cascades10

include(config.pri)

PRE_TARGETDEPS ~= s/.*SalesforceSDK.+/
CONFIG += salesforcesdk

LIBS += -lbbdata
