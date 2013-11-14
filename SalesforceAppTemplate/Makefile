QMAKE_TARGET  = SalesforceAppTemplate
PROJECT_DIR	  := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
I18N_DIR	  := $(PROJECT_DIR)/translations

QMAKEFEATURES = $(SALESFORCE_SDK_LOC)
export QMAKEFEATURES

include mk/cs-base.mk

