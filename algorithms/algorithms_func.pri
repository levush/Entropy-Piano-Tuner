include(../entropypianotuner_config.pri)

defineReplace(declareAlgorithm) {
    algorithmName = $$1

    TEMPLATE = lib

    contains(EPT_CONFIG, allstatic) {
        CONFIG += staticlib
    } else {
        CONFIG += dll
    }

    CONFIG += c++11

    DESTDIR = $$EPT_ALGORITHMS_OUT_DIR

    INCLUDEPATH += $$EPT_BASE_DIR $$EPT_CORE_DIR

    # library
    LIBS += -L$$EPT_CORE_OUT_DIR
    LIBS += -lcore

    # files
    SOURCES += $${algorithmName}.cpp
    HEADERS += $${algorithmName}.h

    DISTFILES += $${algorithmName}.xml

    # Resources (write resource file containing the xml file)
    RESOURCES_FILE_TEXT =                    \
"<RCC>"                                      \
"    <qresource prefix=\"/algorithms\">"     \
"        <file>$${algorithmName}.xml</file>" \
"    </qresource>"                           \
"</RCC>"

    write_file($${algorithmName}.qrc, RESOURCES_FILE_TEXT)

    RESOURCES += $${algorithmName}.qrc

    # export all variables
    export(TEMPLATE)
    export(CONFIG)
    export(DESTDIR)
    export(INCLUDEPATH)
    export(LIBS)
    export(SOURCES)
    export(HEADERS)
    export(DISTFILES)
    export(RESOURCES)
    export(QMAKE_CXXFLAGS_DEBUG)

    # required return value is always true
    return(true)
}
