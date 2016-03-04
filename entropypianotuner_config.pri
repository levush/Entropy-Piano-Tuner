#------------------------------------------------
# Path defines

EPT_ROOT_DIR = $$PWD

EPT_BASE_DIR = $$EPT_ROOT_DIR/base
EPT_APPSTORE_DIR = $$EPT_ROOT_DIR/appstore
EPT_THIRDPARTY_DIR = $$EPT_ROOT_DIR/thirdparty
EPT_DEPENDENCIES_DIR = $$EPT_ROOT_DIR/dependencies
EPT_TRANSLATIONS_DIR = $$EPT_BASE_DIR/translations
EPT_TUTORIAL_DIR = $$EPT_ROOT_DIR/tutorial

EPT_THIRDPARTY_OUT_DIR = $$shadowed($$PWD)/lib
EPT_ALGORITHMS_OUT_DIR = $$shadowed($$PWD)/algorithms


#------------------------------------------------
# third party modules
CONFIG += tinyxml2 qwt

win32 {
  CONFIG += timesupport memorysize rtmidi
}

linux {
  CONFIG += getmemorysize rtmidi
}

macx {
  CONFIG += getmemorysize rtmidi
}

