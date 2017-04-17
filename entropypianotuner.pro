# Global includes
include($$PWD/entropypianotuner_config.pri)
include($$PWD/entropypianotuner_func.pri)

#-------------------------------------------------
#               EPT MODULES
#-------------------------------------------------

TEMPLATE = subdirs

SUBDIRS = \
    app \
    modules \
    thirdparty \

app.depends = modules thirdparty
modules.depends = thirdparty

# Global configuration
DISTFILES += .qmake.conf

#-------------------------------------------------
#                  TRANSLATIONS
#-------------------------------------------------


TRANSLATIONS = \
    $$EPT_TRANSLATIONS_DIR/piano_tuner_de.ts \
    $$EPT_TRANSLATIONS_DIR/piano_tuner_es.ts \
    $$EPT_TRANSLATIONS_DIR/piano_tuner_pl.ts \
    $$EPT_TRANSLATIONS_DIR/piano_tuner_pt.ts \
    $$EPT_TRANSLATIONS_DIR/piano_tuner_ru.ts \
    $$EPT_TRANSLATIONS_DIR/piano_tuner_ko.ts \
    $$EPT_TRANSLATIONS_DIR/piano_tuner_zh.ts \
    $$EPT_TRANSLATIONS_DIR/piano_tuner_fr.ts \

#-------------------------------------------------
#             CONFIGURATION OUTPUT
#-------------------------------------------------

message(CONFIG: $$CONFIG)
message(EPT_CONFIG: $$EPT_CONFIG)
message(EPT_THIRDPARTY_CONFIG: $$EPT_THIRDPARTY_CONFIG)
