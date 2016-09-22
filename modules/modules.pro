TEMPLATE = subdirs

SUBDIRS = \
    algorithms \
    core \
    midi \

algorithms.depends = core
core.depends = midi
