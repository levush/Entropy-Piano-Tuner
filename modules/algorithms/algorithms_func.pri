include(../../entropypianotuner_func.pri)

defineReplace(declareAlgorithm) {
    algorithmName = $$1
    VERSION = $$2

    DEFINES += "EPT_ALGORITHM_NAME=\\\"$${algorithmName}\\\""
    DEFINES += "EPT_ALGORITHM_VERSION_NAME=\\\"$${VERSION}\\\""

    TEMPLATE = lib

    contains(EPT_CONFIG, static_algorithms) {
        CONFIG += staticlib
    } else {
        CONFIG += plugin
    }

    TARGET = $$algorithmName
    CONFIG += c++11

    DESTDIR = $$EPT_ALGORITHMS_OUT_DIR

    INCLUDEPATH += $$EPT_BASE_DIR $$EPT_MODULES_DIR $$EPT_CORE_DIR
    INCLUDEPATH += $$EPT_THIRDPARTY_DIR/tp3log

    # library
    $$depends_core()

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

    # installation of shared lib
    contains(EPT_CONFIG, install):contains(CONFIG, plugin) {
        # add the install dir for the core the the rpath
        unix:QMAKE_RPATHDIR += $$EPT_INSTALL_LIB_RDIR/entropypianotuner


        target.path = $$EPT_INSTALL_LIB_DIR/entropypianotuner/algorithms

        INSTALLS += target

        export(QMAKE_RPATHDIR)
        export(target.path)
        export(INSTALLS)
    }

    # export all variables
    export(TEMPLATE)
    export(CONFIG)
    export(DESTDIR)
    export(INCLUDEPATH)
    export(LIBS)
    export(SOURCES)
    export(HEADERS)
    export(DEFINES)
    export(DISTFILES)
    export(RESOURCES)
    export(QMAKE_CXXFLAGS_DEBUG)

    # required return value is always true
    return(true)
}
