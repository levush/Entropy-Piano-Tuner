include($$PWD/entropypianotuner_config.pri)
include($$PWD/entropypianotuner_func.pri)

message(EPT Configuration: $$CONFIG)
message(EPT 3rdparty configuration: $$EPT_THIRDPARTY_CONFIG)

TEMPLATE = subdirs

SUBDIRS = \
    app \
    modules \
    thirdparty \

app.depends = modules thirdparty
modules.depends = thirdparty

DISTFILES += .qmake.conf

message(Config: $$CONFIG)
message(EPT_CONFIG: $$EPT_CONFIG)
message(EPT_THIRDPARTY_CONFIG: $$EPT_THIRDPARTY_CONFIG)
