#-------------------------------------------------
#
# Entropy Piano Tuner
#
# Project created by QtCreator 2015-02-09T15:03:29
#
#-------------------------------------------------

# Qt modules
QT          += core gui multimedia widgets concurrent midi
qtHaveModule(svg):QT += svg
android:QT  += androidextras

# Target and config
TARGET = entropypianotuner
TEMPLATE = app
CONFIG += c++11

include(../entropypianotuner_config.pri)
include(../entropypianotuner_func.pri)

# path defines

INCLUDEPATH += $$EPT_DEPENDENCIES_DIR/include
INCLUDEPATH += $$EPT_THIRDPARTY_DIR
INCLUDEPATH += $$EPT_BASE_DIR $$EPT_ROOT_DIR $$EPT_MODULES_DIR
INCLUDEPATH += $$EPT_THIRDPARTY_DIR/tp3log

DESTDIR = $$EPT_TARGET_OUT_DIR

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
# when we statically link the alogrithms, we create a c++ file that
# will instantiate the algorithms (create a static variable of each AlgorithmFactory)
# the algorithm will automatically add itself to the CalculationManager
# if the Constructor is called.
# !build_pass ensures that the file generation is only executed once
include($$EPT_ALGORITHMS_DIR/algorithms_config.pri)
contains(EPT_CONFIG, static_algorithms) {
    # define the output c++ file that will be added to the source,
    # but only created once during the build_pass
    OUT_FILE = $$OUT_PWD/algorithms.gen.cpp

    # create the static cpp file that initilizes all algorithms
    !build_pass {
        ALG_FILE_CPP = "// This file was generated automatically"

        for(algBasename, ALGORITHM_NAMES) {
            message(Adding algorithm $$algBasename)

            ALG_FILE_CPP = $$join(ALG_FILE_CPP,,,"$${escape_expand(\n)}$${LITERAL_HASH}include \"$${algBasename}/$${algBasename}.h\"")
            ALG_FILE_CPP = $$join(ALG_FILE_CPP,,,"$${escape_expand(\n)}static $${algBasename}::Factory $${algBasename}_FACTORY($${algBasename}::getInitFactoryDescription());$${escape_expand(\n)}")
        }


        write_file($$OUT_FILE, ALG_FILE_CPP)
    }

    # add to sources, and add include/library paths and libs
    SOURCES += $$OUT_FILE
    INCLUDEPATH += $$EPT_ALGORITHMS_DIR
    LIBS += -L$$EPT_ALGORITHMS_OUT_DIR
    for(algBasename, ALGORITHM_NAMES) {
        LIBS += -l$$algBasename
    }
}


#-------------------------------------------------
#                  Thirdparty dependencies
#-------------------------------------------------

# swap when using the system fftw3 (on linux)
# I dont know the reason, but elsewise there are
# linker errors
contains(EPT_THIRDPARTY_CONFIG, system_fftw3) {
    # this is the order required for linux (system lib)
    $$depends_core()
    $$depends_fftw3()
} else {
    # this is the order required for android
    $$depends_fftw3()
    $$depends_core()
}

$$depends_getmemorysize()
$$depends_libuv()
$$depends_qwt()
$$depends_timesupport()

#-------------------------------------------------
#                      Apple
#-------------------------------------------------

# MacOS X (no iOS)
macx {
    LIBS += -framework CoreFoundation -framework CoreAudio -framework CoreMidi -framework IOKit

    # icons
    ICON = $$EPT_ROOT_DIR/appstore/icons/entropytuner.icns

    # set plist file
    QMAKE_INFO_PLIST = $$PWD/platforms/osx/Info.plist
}

# iOS
ios {
    # the following line is needed with XCode 7 bug 58926
    QMAKE_MAC_SDK = iphoneos

    # team settings (xcode 8)
    QMAKE_MAC_XCODE_SETTINGS=team team.name=DEVELOPMENT_TEAM team.value=W79N63VU7K 

    # set plist file
    QMAKE_INFO_PLIST = $$PWD/platforms/ios/Info.plist

    # app icons
    ios_icon.files = $$files($$EPT_BASE_DIR/appstore/icons/ios/AppIcon*.png)
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
    WINRT_MANIFEST.identity = "28869HayeHinrichsen.EntropyPianoTuner"
    WINRT_MANIFEST.publisherid = "CN=1A48CBB8-649C-4CBD-908D-A767026541C7"
    WINRT_MANIFEST.name = "Entropy Piano Tuner"
    WINRT_MANIFEST.publisher = "Haye Hinrichsen"
    WINRT_MANIFEST.logo_large = $$EPT_APPSTORE_DIR/icons/winrt/icon_150x150.png
    WINRT_MANIFEST.logo_small = $$EPT_APPSTORE_DIR/icons/winrt/icon_30x30.png
    WINRT_MANIFEST.logo_store = $$EPT_APPSTORE_DIR/icons/winrt/icon_50x50.png
    WINRT_MANIFEST.logo_splash = $$EPT_APPSTORE_DIR/splash/splash_620x300.png
    WINRT_MANIFEST.background = $${LITERAL_HASH}e5e5e5
    WINRT_MANIFEST.version = 1.2.0.0
    WINRT_MANIFEST.logo_medium = $$EPT_APPSTORE_DIR/icons/winrt/icon_100x100.png
    WINRT_MANIFEST.tile_iconic_small = $$EPT_APPSTORE_DIR/icons/winrt/icon_100x100.png
    WINRT_MANIFEST.tile_iconic_medium = $$EPT_APPSTORE_DIR/icons/winrt/icon_100x100.png
    WINRT_MANIFEST.capabilities_device = microphone
    WINRT_MANIFEST = $$EPT_ROOT_DIR/scripts/windowsstore/AppxManifest.xml.in

    WINDEPLOYQT_OPTIONS = -qmldir $$shell_quote($$system_path($$_PRO_FILE_PWD_))

    CONFIG += windeployqt

    # add winrtbridge to required dlls for copying
    DLLS += $$EPT_THIRDPARTY_OUT_DIR/winrtbridge.dll

    # need to copy the midi plugin manually on win rt
    CONFIG(debug, debug|release) {
        PLUGIN_DIR = $$DESTDIR
        PLUGIN_FILE=qtwinrt_midid.dll
    }
    else {
        PLUGIN_DIR = $$DESTDIR
        PLUGIN_FILE=qtwinrt_midi.dll
    }

    PLUGIN_DIR ~= s,/,\\,g

    QMAKE_POST_LINK += \
        $$sprintf($$QMAKE_MKDIR_CMD, $$quote($$PLUGIN_DIR\\midi)) $$escape_expand(\\n\\t) \
        $$QMAKE_COPY $$quote($$(QTDIR)\\plugins\\midi\\$$PLUGIN_FILE) $$quote($$PLUGIN_DIR\\midi\\$$PLUGIN_FILE) $$escape_expand(\\n\\t)
} else:winphone {
} else:win32 {
    # windows desktop
}

# dlls
winrt|winphone|win32|win32-g++ {
    # copy external dlls
    DLLS ~= s,/,\\,g
    DESTDIR_WIN = $$DESTDIR
    DESTDIR_WIN ~= s,/,\\,g
    for(FILE,DLLS){
        # skip files, that are already built in the currect destination
        !equals(FILE, $${DESTDIR_WIN}\\$$basename(FILE)) {
            QMAKE_POST_LINK += $$quote(cmd /c $$QMAKE_COPY $${FILE} $${DESTDIR_WIN} $$escape_expand(\n\t))
        }
    }
} else:linux {
    # copy so's, that they are found by the executable
    for (FILE,DLLS) {
        QMAKE_POST_LINK += $$quote($$QMAKE_COPY $${FILE} $${EPT_TARGET_OUT_DIR} $$escape_expand(\n\t))
    }
}



#-------------------------------------------------
#                      Linux
#-------------------------------------------------


# linux libs
linux-g++*:!android {
    # additional defines in debug modus
    QMAKE_CXXFLAGS_DEBUG += -D_GLIBCXX_DEBUG -Wall -Werror -Wpedantic
}

#-------------------------------------------------
#               Files to build the app
#-------------------------------------------------


HEADERS  += \
    audioforqt/audiointerfaceforqt.h \
    audioforqt/audioplayerforqt.h \
    audioforqt/audiorecorderforqt.h \
    audioforqt/qtpcmdevice.h \
    dialogs/algorithmdialog/algorithmdialog.h \
    dialogs/algorithmdialog/algorithmdialogparameterupdatetimer.h \
    dialogs/editpianosheet/editpianosheetdialog.h \
    dialogs/log/logviewer.h \
    dialogs/plotsdialog/centralplotframe.h \
    dialogs/plotsdialog/keyindexscaledraw.h \
    dialogs/plotsdialog/keyindexscaleengine.h \
    dialogs/plotsdialog/plotsdialog.h \
    dialogs/aboutdialog.h \
    dialogs/autoclosingmessagebox.h \
    dialogs/donotshowagainmessagebox.h \
    dialogs/initializedialog.h \
    dialogs/simplefiledialog.h \
    drawers/fourierspectrumgraph.h \
    drawers/stroboscopicviewadapterforqt.h \
    drawers/tuningcurvegraph.h \
    drawers/tuningindicatorview.h \
    implementations/filemanagerforqt.h \
    implementations/graphicsitemforqt.h \
    implementations/graphicsviewadapterforqt.h \
    implementations/platformtools.h \
    implementations/projectmanagerforqt.h \
    implementations/qtxmlreader.h \
    implementations/qtxmlwriter.h \
    implementations/settingsforqt.h \
    keyboard/autoscaledtokeyboardgraphicsview.h \
    keyboard/fullscreenkeyboarddialog.h \
    keyboard/graphicskeyitem.h \
    keyboard/keyboardgraphicsview.h \
    mainwindow/calculationprogressgroup.h \
    mainwindow/mainwindow.h \
    mainwindow/progressdisplay.h \
    mainwindow/signalanalyzergroupbox.h \
    mainwindow/tuningindicatorgroupbox.h \
    mainwindow/recordingqualitybar.h \
    mainwindow/recordingstatusgraphicsview.h \
    mainwindow/volumecontrolgroupbox.h \
    mainwindow/volumecontrollevel.h \
    options/audio/optionspageaudio.h \
    options/audio/optionspageaudiomidipage.h \
    options/audio/optionspageaudioinputoutputpage.h \
    options/environment/optionspageenvironment.h \
    options/environment/optionspageenvironmentgeneralpage.h \
    options/environment/optionspageenvironmenttuningpage.h \
    options/optionscentralwidgetinterface.h \
    options/optionscontentswidgetinterface.h \
    options/optionsdialog.h \
    options/optionstabcontentsvscrollarea.h \
    options/pagesavingtabwidget.h \
    piano/pianofileiointerface.h \
    piano/pianofileioxml.h \
    piano/pianofileiocsv.h \
    widgets/displaysizedependinggroupbox.h \
    widgets/doubleslider.h \
    widgets/preferredtextsizelabel.h \
    widgets/progressoverlay.h \
    widgets/verticalscrollarea.h \
    displaysize.h \
    prerequisites.h \
    qtconfig.h \
    runguard.h \
    tunerapplication.h \
    versioncheck.h \


SOURCES +=  \
    audioforqt/audiointerfaceforqt.cpp \
    audioforqt/audioplayerforqt.cpp \
    audioforqt/audiorecorderforqt.cpp \
    audioforqt/qtpcmdevice.cpp \
    dialogs/algorithmdialog/algorithmdialog.cpp \
    dialogs/algorithmdialog/algorithmdialogparameterupdatetimer.cpp \
    dialogs/editpianosheet/editpianosheetdialog.cpp \
    dialogs/log/logviewer.cpp \
    dialogs/plotsdialog/plotsdialog.cpp \
    dialogs/plotsdialog/centralplotframe.cpp \
    dialogs/plotsdialog/keyindexscaleengine.cpp \
    dialogs/plotsdialog/keyindexscaledraw.cpp \
    dialogs/aboutdialog.cpp \
    dialogs/autoclosingmessagebox.cpp \
    dialogs/donotshowagainmessagebox.cpp \
    dialogs/initializedialog.cpp \
    dialogs/simplefiledialog.cpp \
    drawers/fourierspectrumgraph.cpp \
    drawers/stroboscopicviewadapterforqt.cpp \
    drawers/tuningcurvegraph.cpp \
    drawers/tuningindicatorview.cpp \
    implementations/filemanagerforqt.cpp \
    implementations/graphicsitemforqt.cpp \
    implementations/graphicsviewadapterforqt.cpp \
    implementations/platformtools.cpp \
    implementations/projectmanagerforqt.cpp \
    implementations/qtxmlreader.cpp \
    implementations/qtxmlwriter.cpp \
    implementations/settingsforqt.cpp \
    keyboard/autoscaledtokeyboardgraphicsview.cpp \
    keyboard/fullscreenkeyboarddialog.cpp \
    keyboard/graphicskeyitem.cpp \
    keyboard/keyboardgraphicsview.cpp \
    mainwindow/calculationprogressgroup.cpp \
    mainwindow/mainwindow.cpp \
    mainwindow/progressdisplay.cpp \
    mainwindow/recordingqualitybar.cpp \
    mainwindow/recordingstatusgraphicsview.cpp \
    mainwindow/signalanalyzergroupbox.cpp \
    mainwindow/tuningindicatorgroupbox.cpp \
    mainwindow/volumecontrolgroupbox.cpp \
    mainwindow/volumecontrollevel.cpp \
    options/audio/optionspageaudio.cpp \
    options/audio/optionspageaudiomidipage.cpp \
    options/audio/optionspageaudioinputoutputpage.cpp \
    options/environment/optionspageenvironment.cpp \
    options/environment/optionspageenvironmentgeneralpage.cpp \
    options/environment/optionspageenvironmenttuningpage.cpp \
    options/optionscentralwidgetinterface.cpp \
    options/optionscontentswidgetinterface.cpp \
    options/optionsdialog.cpp \
    options/optionstabcontentsvscrollarea.cpp \
    options/pagesavingtabwidget.cpp \
    piano/pianofileiointerface.cpp \
    piano/pianofileioxml.cpp \
    piano/pianofileiocsv.cpp \
    widgets/displaysizedependinggroupbox.cpp \
    widgets/doubleslider.cpp \
    widgets/preferredtextsizelabel.cpp \
    widgets/progressoverlay.cpp \
    widgets/verticalscrollarea.cpp \
    displaysize.cpp \
    main.cpp \
    runguard.cpp \
    tunerapplication.cpp \
    versioncheck.cpp \


#-------------------------------------------------
#            Platform specific includes
#-------------------------------------------------


# add android files
android {
    include($$PWD/platforms/android/android.pri)
}

# add ios files
ios {
    include($$PWD/platforms/ios/ios.pri)
}

# add macx files
macx {
    include($$PWD/platforms/osx/osx.pri)
}

# add windows files
winrt {
    include($$PWD/platforms/winrt/winrt.pri)
}else:win32 {
    include($$PWD/platforms/windows/windows.pri)
}

#-------------------------------------------------
#               Forms and Resources
#-------------------------------------------------

FORMS += \
    dialogs/editpianosheet/editpianosheetdialog.ui \
    dialogs/log/logviewer.ui \
    mainwindow/mainwindow.ui \


RESOURCES += \
    $$EPT_BASE_DIR/media/media.qrc \
    $$EPT_TUTORIAL_DIR/tutorial.qrc \
    $$EPT_TRANSLATIONS_DIR/languages.qrc



#-------------------------------------------------
#                      INSTALL
#-------------------------------------------------

contains(EPT_CONFIG, install) {
    # add the install dir for the core the the rpath
    unix:QMAKE_RPATHDIR += $$EPT_INSTALL_LIB_RDIR/entropypianotuner

    target.path = $$EPT_INSTALL_BIN_DIR

    pixmaps.path = $$EPT_INSTALL_DATA_DIR/pixmaps
    pixmaps.files += $$EPT_APPSTORE_DIR/icons/entropypianotuner.png

    icons.path = $$EPT_INSTALL_DATA_DIR/icons/hicolor/128x128/mimetypes
    icons.files += $$EPT_APPSTORE_DIR/icons/application-ept.png

    mime.path = $$EPT_INSTALL_DATA_DIR/mime/packages
    mime.files += $$EPT_APPSTORE_DIR/installer/scripts/entropypianotuner-mime.xml

    application.path = $$EPT_INSTALL_DATA_DIR/applications
    application.files += $$EPT_APPSTORE_DIR/installer/scripts/entropypianotuner.desktop

    !contains(EPT_THIRDPARTY_CONFIG, system_qwt):!contains(EPT_CONFIG, static_qwt) {
        # install target for qwt
        qwtinstall.path = $$EPT_INSTALL_LIB_DIR/entropypianotuner

        qwt_files = $$files($$EPT_THIRDPARTY_OUT_DIR/libqwt*)
        for(file, qwt_files):qwtinstall.files += $$file

        INSTALLS += qwtinstall
    }

    !contains(EPT_THIRDPARTY_CONFIG, system_fftw3):!contains(EPT_CONFIG, static_fftw) {
        # install target for fftw3
        fftwinstall.path = $$EPT_INSTALL_LIB_DIR/entropypianotuner

        fftw_files = $$files($$EPT_THIRDPARTY_OUT_DIR/libfftw3*)
        for(file, fftw_files):fftwinstall.files += $$file

        INSTALLS += fftwinstall
    }

    INSTALLS += target pixmaps icons mime application
}
