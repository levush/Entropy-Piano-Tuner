include($$PWD/entropypianotuner_config.pri)

TEMPLATE = subdirs

SUBDIRS = \
    base \
    algorithms \
    thirdparty \


algorithms.depends = base
base.depends = thirdparty
