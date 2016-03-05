include($$PWD/entropypianotuner_config.pri)

TEMPLATE = subdirs

SUBDIRS = \
    app \
    core \
    algorithms \
    thirdparty \

app.depends = core algorithms thirdparty
core.depends = thirdparty
algorithms.depends = core
