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
