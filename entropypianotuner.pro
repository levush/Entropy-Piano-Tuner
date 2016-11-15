include($$PWD/entropypianotuner_config.pri)
include($$PWD/entropypianotuner_func.pri)

message(EPT Configuration: $$CONFIG)
message(EPT 3rdparty configuration: $$EPT_THIRDPARTY_CONFIG)

TEMPLATE = subdirs

SUBDIRS = \
    app \
    core \
    algorithms \
    thirdparty \

app.depends = core algorithms thirdparty
core.depends = thirdparty
algorithms.depends = core
