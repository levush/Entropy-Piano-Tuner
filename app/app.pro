#-------------------------------------------------
#
# Entropy Piano Tuner
#
# Project created by QtCreator 2015-02-09T15:03:29
#
#-------------------------------------------------

QT       += core gui multimedia svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = entropypianotuner
TEMPLATE = app
CONFIG += c++11

include(../entropypianotuner_config.pri)
include(../entropypianotuner_func.pri)

# path defines

INCLUDEPATH += $$EPT_DEPENDENCIES_DIR/include
INCLUDEPATH += $$EPT_THIRDPARTY_DIR
INCLUDEPATH += $$EPT_BASE_DIR $$EPT_ROOT_DIR $$EPT_CORE_DIR

DESTDIR = $$EPT_CORE_OUT_DIR

Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui

#-------------------------------------------------
#                    ALGORITHMS
#-------------------------------------------------
# link if static build
contains(EPT_CONFIG, allstatic) {
    LIBS += -L$$EPT_ALGORITHMS_OUT_DIR
    INCLUDEPATH += $$EPT_ALGORITHMS_DIR

    ALG_FILE_CPP = "// This file was generated automatically"

    algorithmDirs = $$files($$EPT_ALGORITHMS_DIR/*)
    for(algorithmDir, algorithmDirs) {
        algorithmFiles = $$files($$algorithmDir/*)
        for(algorithmFile, algorithmFiles) {
            res = $$find(algorithmFile, "\.pro")
            !isEmpty(res) {
                algBasename = $$basename(res)
                algBasename = $$replace(algBasename, "\.pro", "")
                message(Adding algorithm $$algBasename)
                LIBS += -l$$algBasename

                ALG_FILE_CPP = $$join(ALG_FILE_CPP,,,"$${escape_expand(\n)}$${LITERAL_HASH}include \"$${algBasename}/$${algBasename}.h\"")
                ALG_FILE_CPP = $$join(ALG_FILE_CPP,,,"$${escape_expand(\n)}static $${algBasename}::Factory $${algBasename}_FACTORY($${algBasename}::getInitFactoryDescription());$${escape_expand(\n)}")
            }
        }
    }

    OUT_FILE = $$OUT_PWD/algorithms.gen.cpp

    write_file($$OUT_FILE, ALG_FILE_CPP)
    SOURCES += $$OUT_FILE
}


# core lib
LIBS += -L$$EPT_CORE_OUT_DIR -lcore

#-------------------------------------------------
#                  Thirdparty dependencies
#-------------------------------------------------
$$depends_fftw3()
$$depends_getmemorysize()
$$depends_libuv()
$$depends_qwt()
$$depends_rtmidi()
$$depends_timesupport()
$$depends_tinyxml2()


#-------------------------------------------------
#                      Apple
#-------------------------------------------------

# MacOS X (no iOS)
macx {
    LIBS += -L$$PWD/dependencies/lib/macos -lfftw3
    LIBS += -framework CoreFoundation -framework CoreAudio -framework CoreMidi

    # icons
    ICON = $$PWD/appstore/icons/entropytuner.icns

    # set plist file
    QMAKE_INFO_PLIST = $$PWD/platforms/osx/Info.plist
}

# iOS
ios {
    # the following line is needed with XCode 7 bug 58926
    QMAKE_MAC_SDK = iphoneos

    iphonesimulator {
        LIBS += -L$$PWD/dependencies/lib/macos -lfftw3
    }
    iphoneos {
        LIBS += -L$$PWD/dependencies/lib/ios -lfftw3_armv7
        # in release mode also arm64 is required
        LIBS += -L$$PWD/dependencies/lib/ios -lfftw3_arm64
    }


    # set plist file
    QMAKE_INFO_PLIST = $$PWD/platforms/ios/Info.plist

    # app icons
    ios_icon.files = $$files($$PWD/appstore/icons/ios/AppIcon*.png)
    QMAKE_BUNDLE_DATA += ios_icon

    # lauch screen file
    launch_screen.files = $$PWD/platforms/ios/Launch.xib
    QMAKE_BUNDLE_DATA += launch_screen

    # Set "Devices" (1=iPhone, 2=iPad, 1,2=Universal)
    isEmpty(QMAKE_IOS_TARGETED_DEVICE_FAMILY) {
        QMAKE_IOS_TARGETED_DEVICE_FAMILY = 2
    }
}


#-------------------------------------------------
#                      Windows
#-------------------------------------------------


win32 {
    RC_ICONS = $$EPT_APPSTORE_DIR/icons/entropytuner.ico
    DEFINES += NOMINMAX
}

winrt {
    WINRT_MANIFEST.logo_large = $$EPT_APPSTORE_DIR/icons/winrt/icon_150x150.png
    WINRT_MANIFEST.logo_small = $$EPT_APPSTORE_DIR/icons/winrt/icon_30x30.png
    WINRT_MANIFEST.logo_store = $$EPT_APPSTORE_DIR/icons/winrt/icon_50x50.png
    WINRT_MANIFEST.logo_splash = $$EPT_APPSTORE_DIR/splash/splash_620x300.png
    WINRT_MANIFEST.background = $${LITERAL_HASH}e5e5e5
    WINRT_MANIFEST.publisher = "Haye Hinrichsen"
    winphone:equals(WINSDK_VER, 8.1) {
        WINRT_MANIFEST.logo_medium = $$EPT_APPSTORE_DIR/icons/winrt/icon_100x100.png
        WINRT_MANIFEST.tile_iconic_small = $$EPT_APPSTORE_DIR/icons/winrt/icon_100x100.png
        WINRT_MANIFEST.tile_iconic_medium = $$EPT_APPSTORE_DIR/icons/winrt/icon_100x100.png
    } else {
    }
    CONFIG += windeployqt
    WINDEPLOYQT_OPTIONS = -no-svg -qmldir $$shell_quote($$system_path($$_PRO_FILE_PWD_))
} else:winphone {
} else:win32 {
    # windows desktop

    # windows multimedia is required for rt midi
    LIBS += -lwinmm
}

# dlls
win32|win32-g++ {
    # copy external dlls
    DLLS ~= s,/,\\,g
    DESTDIR_WIN = $$OUT_PWD/$$DESTDIR
    DESTDIR_WIN ~= s,/,\\,g
    for(FILE,DLLS){
        QMAKE_POST_LINK += $$quote(cmd /c $$QMAKE_COPY $${FILE} $${DESTDIR_WIN} $$escape_expand(\n\t))
    }
}



#-------------------------------------------------
#                      Linux
#-------------------------------------------------


# linux libs
linux-g++*:!android {
    LIBS += -lasound

    # older version needs explicit cxx flag
    QMAKE_CXXFLAGS += -std=c++11

    # additional defines in debug modus
    QMAKE_CXXFLAGS_DEBUG += -D_GLIBCXX_DEBUG -Wall -Werror -Wpedantic
}
# android libs
android {
    QT += androidextras
}

#-------------------------------------------------
#                   Qt files
#-------------------------------------------------


HEADERS  += \
    plotsdialog/centralplotframe.h \
    plotsdialog/keyindexscaleengine.h \
    plotsdialog/keyindexscaledraw.h \
    preferredtextsizelabel.h \
    verticalscrollarea.h \
    options/optionstabcontentsvscrollarea.h \
    tuningindicatorview.h \
    stroboscopicviewadapterforqt.h \
    tuningindicatorgroupbox.h \
    audioforqt/audioplayerthreadforqt.h \
    mainwindow.h \
    volumecontrollevel.h \
    tunerapplication.h \
    fourierspectrumgraph.h \
    tuningcurvegraph.h \
    keyboard/autoscaledtokeyboardgraphicsview.h \
    graphicsviewadapterforqt.h \
    projectmanagerforqt.h \
    logforqt.h \
    audioforqt/audioplayerforqt.h \
    audioforqt/audiorecorderforqt.h \
    filemanagerforqt.h \
    initializedialog.h \
    logviewer.h \
    recordingstatusgraphicsview.h \
    settingsforqt.h \
    options/optionsdialog.h \
    keyboard/keyboardgraphicsview.h \
    options/optionscentralwidgetinterface.h \
    options/optionscontentswidgetinterface.h \
    options/audio/optionspageaudio.h \
    options/audio/optionspageaudiomidipage.h \
    options/audio/optionspageaudioinputoutputpage.h \
    options/environment/optionspageenvironment.h \
    options/environment/optionspageenvironmentgeneralpage.h \
    platformtools.h \
    donotshowagainmessagebox.h \
    keyboard/fullscreenkeyboarddialog.h \
    calculationprogressgroup.h \
    recordingqualitybar.h \
    autoclosingmessagebox.h \
    graphicsitemforqt.h \
    editpianosheetdialog.h \
    options/environment/optionspageenvironmenttuningpage.h \
    doubleslider.h \
    simplefiledialog.h \
    algorithmdialog/algorithmdialog.h \
    algorithmdialog/algorithmdialogparameterupdatetimer.h \
    qtconfig.h \
    keyboard/graphicskeyitem.h \
    aboutdialog.h \
    versioncheck.h \
    runguard.h \
    displaysize.h \
    displaysizedependinggroupbox.h \
    signalanalyzergroupbox.h \
    volumecontrolgroupbox.h \
    plotsdialog/plotsdialog.h \
    progressdisplay.h \
    options/pagesavingtabwidget.h \
    piano/pianofileiointerface.h \
    piano/pianofileioxml.h \
    piano/pianofileiocsv.h \
    prerequisites.h

SOURCES +=  \
    main.cpp\
    mainwindow.cpp \
    volumecontrollevel.cpp \
    tunerapplication.cpp \
    fourierspectrumgraph.cpp \
    tuningcurvegraph.cpp \
    keyboard/autoscaledtokeyboardgraphicsview.cpp \
    graphicsviewadapterforqt.cpp \
    projectmanagerforqt.cpp \
    logforqt.cpp \
    audioforqt/audioplayerforqt.cpp \
    audioforqt/audiorecorderforqt.cpp \
    filemanagerforqt.cpp \
    initializedialog.cpp \
    logviewer.cpp \
    recordingstatusgraphicsview.cpp \
    settingsforqt.cpp \
    options/optionsdialog.cpp \
    keyboard/keyboardgraphicsview.cpp \
    options/optionscentralwidgetinterface.cpp \
    options/optionscontentswidgetinterface.cpp \
    options/audio/optionspageaudio.cpp \
    options/audio/optionspageaudiomidipage.cpp \
    options/audio/optionspageaudioinputoutputpage.cpp \
    options/environment/optionspageenvironment.cpp \
    options/environment/optionspageenvironmentgeneralpage.cpp \
    platformtools.cpp \
    donotshowagainmessagebox.cpp \
    keyboard/fullscreenkeyboarddialog.cpp \
    calculationprogressgroup.cpp \
    recordingqualitybar.cpp \
    autoclosingmessagebox.cpp \
    graphicsitemforqt.cpp \
    editpianosheetdialog.cpp \
    options/environment/optionspageenvironmenttuningpage.cpp \
    doubleslider.cpp \
    simplefiledialog.cpp \
    algorithmdialog/algorithmdialog.cpp \
    algorithmdialog/algorithmdialogparameterupdatetimer.cpp \
    keyboard/graphicskeyitem.cpp \
    aboutdialog.cpp \
    versioncheck.cpp \
    runguard.cpp \
    displaysize.cpp \
    displaysizedependinggroupbox.cpp \
    signalanalyzergroupbox.cpp \
    volumecontrolgroupbox.cpp \
    plotsdialog/plotsdialog.cpp \
    plotsdialog/centralplotframe.cpp \
    plotsdialog/keyindexscaleengine.cpp \
    plotsdialog/keyindexscaledraw.cpp \
    preferredtextsizelabel.cpp \
    verticalscrollarea.cpp \
    options/optionstabcontentsvscrollarea.cpp \
    tuningindicatorview.cpp \
    stroboscopicviewadapterforqt.cpp \
    tuningindicatorgroupbox.cpp \
    audioforqt/audioplayerthreadforqt.cpp \
    progressdisplay.cpp \
    options/pagesavingtabwidget.cpp \
    piano/pianofileiointerface.cpp \
    piano/pianofileioxml.cpp \
    piano/pianofileiocsv.cpp \


#-------------------------------------------------
#                   ANDROID
#-------------------------------------------------


# add android files
android {
    include($$EPT_BASE_DIR/platforms/android/android.pri)
}

# add ios files
ios {
    include($$EPT_BASE_DIR/platforms/ios/ios.pri)
}

# add macx files
macx {
    include($$EPT_BASE_DIR/platforms/osx/osx.pri)
}

# add windows files
win32 {
    include($$EPT_BASE_DIR/platforms/windows/windows.pri)
}

# add applenativewrapper files
mac {
    HEADERS +=
    OBJECTIVE_SOURCES +=
}

#-------------------------------------------------
#               Forms and Resources
#-------------------------------------------------

FORMS += \
    mainwindow.ui \
    logviewer.ui \
    editpianosheetdialog.ui

RESOURCES += \
    $$EPT_BASE_DIR/media/media.qrc \
    $$EPT_TUTORIAL_DIR/tutorial.qrc \
    $$EPT_TRANSLATIONS_DIR/languages.qrc



#-------------------------------------------------
#                  TRANSLATIONS
#-------------------------------------------------


TRANSLATIONS = \
    translations/piano_tuner_de.ts \
    translations/piano_tuner_es.ts \
    translations/piano_tuner_pl.ts \
    translations/piano_tuner_pt.ts \
    translations/piano_tuner_ru.ts \
    translations/piano_tuner_ko.ts \
    translations/piano_tuner_zh.ts \
    translations/piano_tuner_fr.ts \


#-------------------------------------------------
#                      INSTALL
#-------------------------------------------------


target.path = $$PREFIX/bin

pixmaps.path = $$PREFIX/share/pixmaps
pixmaps.files += $$PWD/appstore/icons/entropypianotuner.png

icons.path = $$PREFIX/share/icons/hicolor/256x256/mimetypes
icons.files += $$PWD/appstore/icons/application-ept.png

mime.path = $$PREFIX/share/mime/packages
mime.files += $$PWD/appstore/installer/scripts/entropypianotuner-mime.xml

application.path = $$PREFIX/share/applications
application.files += $$PWD/appstore/installer/scripts/entropypianotuner.desktop

INSTALLS += target pixmaps icons mime application
