TEMPLATE = subdirs

SUBDIRS = \
    algorithms \
    core \
    umidi \

algorithms.depends = core
core.depends = umidi
