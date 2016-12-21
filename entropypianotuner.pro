include($$PWD/entropypianotuner_config.pri)
include($$PWD/entropypianotuner_func.pri)

TEMPLATE = subdirs

SUBDIRS = \
    app \
    modules \
    thirdparty \

app.depends = modules thirdparty
modules.depends = thirdparty

DISTFILES += .qmake.conf

message(CONFIG: $$CONFIG)
message(EPT_CONFIG: $$EPT_CONFIG)
message(EPT_THIRDPARTY_CONFIG: $$EPT_THIRDPARTY_CONFIG)
