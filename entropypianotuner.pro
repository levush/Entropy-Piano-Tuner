include($$PWD/entropypianotuner_config.pri)
include($$PWD/entropypianotuner_func.pri)

TEMPLATE = subdirs

SUBDIRS = \
    app \
    core \
    algorithms \
    thirdparty \

app.depends = core algorithms thirdparty
core.depends = thirdparty
algorithms.depends = core
