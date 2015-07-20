#-------------------------------------------------
#
# Project created by QtCreator 2015-02-09T15:03:29
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = entropypianotuner
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += dependencies/include
INCLUDEPATH += Qt

Release:DESTDIR = release
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui

# add qwt
QWT_CONFIG += QwtPlot
include($$PWD/thirdparty/qwt/qwt.pri)
QWT_H = $$HEADERS
HEADERS = \
    Qt/plotsdialog/centralplotframe.h \
    Qt/plotsdialog/keyindexscaleengine.h \
    Qt/plotsdialog/keyindexscaledraw.h \
    core/audio/pcmwriterinterface.h \
    core/system/serverinfo.h \
    Qt/preferredtextsizelabel.h \
    Qt/verticalscrollarea.h \
    Qt/options/optionstabcontentsvscrollarea.h
QWT_S = $$SOURCES
SOURCES = \
    Qt/plotsdialog/centralplotframe.cpp \
    Qt/plotsdialog/keyindexscaleengine.cpp \
    Qt/plotsdialog/keyindexscaledraw.cpp \
    core/piano/pianodefines.cpp \
    core/system/serverinfo.cpp \
    Qt/preferredtextsizelabel.cpp \
    Qt/verticalscrollarea.cpp \
    Qt/options/optionstabcontentsvscrollarea.cpp
for(file, QWT_H):HEADERS += $$replace(file, qwt, $$PWD/thirdparty/qwt/qwt)
for(file, QWT_S):SOURCES += $$replace(file, qwt, $$PWD/thirdparty/qwt/qwt)
INCLUDEPATH += $$PWD/thirdparty/qwt

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

win32 {
    RC_ICONS = $$PWD/appstore/icons/entropytuner.ico
    DEFINES += NOMINMAX
}

winrt {
    WINRT_MANIFEST.logo_large = $$PWD/appstore/icons/winrt/icon_150x150.png
    WINRT_MANIFEST.logo_small = $$PWD/appstore/icons/winrt/icon_30x30.png
    WINRT_MANIFEST.logo_store = $$PWD/appstore/icons/winrt/icon_50x50.png
    WINRT_MANIFEST.logo_splash = $$PWD/appstore/splash/splash_620x300.png
    WINRT_MANIFEST.background = $${LITERAL_HASH}e5e5e5
    WINRT_MANIFEST.publisher = "Haye Hinrichsen"
    winphone:equals(WINSDK_VER, 8.1) {
        WINRT_MANIFEST.logo_medium = $$PWD/appstore/icons/winrt/icon_100x100.png
        WINRT_MANIFEST.tile_iconic_small = $$PWD/appstore/icons/winrt/icon_100x100.png
        WINRT_MANIFEST.tile_iconic_medium = $$PWD/appstore/icons/winrt/icon_100x100.png
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

win32:contains(QT_ARCH, i386):{
    # this library is for static linking
    LIBS += -L$$PWD/dependencies/lib/win_i386 -llibfftw3-3
}
win32:contains(QT_ARCH, x86):{
    # this library is for static linking
    LIBS += -L$$PWD/dependencies/lib/win_x86 -llibfftw3-3
}
win32:contains(QT_ARCH, arm):{
    LIBS += -L$$PWD/dependencies/lib/win_arm32 -llibfftw3-3
}
win32:contains(QT_ARCH, x86_64):{
    LIBS += -L$$PWD/dependencies/lib/win_x86_64 -llibfftw3-3

    # copy external dlls
    DLLS = $$PWD/dependencies/bin/win_x86_64/libfftw3-3.dll
    DLLS ~= s,/,\\,g
    DESTDIR_WIN = $$OUT_PWD/$$DESTDIR
    DESTDIR_WIN ~= s,/,\\,g
    for(FILE,DLLS){
        QMAKE_POST_LINK += $$quote(cmd /c $$QMAKE_COPY $${FILE} $${DESTDIR_WIN} $$escape_expand(\n\t))
    }
}

# linux libs
linux-g++*:!android {
    LIBS += -lfftw3 -lasound

    # older version needs explicit cxx flag
    QMAKE_CXXFLAGS += -std=c++11

    # additional defines in debug modus
    QMAKE_CXXFLAGS_DEBUG += -D_GLIBCXX_DEBU -Wall -Werror
}
# android libs
android {
    LIBS += -lfftw3
    QT += androidextras
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/platforms/android
}

#------------------- Qt -----------------------

HEADERS  += \
    Qt/mainwindow.h \
    Qt/volumecontrollevel.h \
    Qt/tunerapplication.h \
    Qt/fourierspectrumgraph.h \
    Qt/tuningcurvegraph.h \
    Qt/keyboard/autoscaledtokeyboardgraphicsview.h \
    Qt/zoomedspectrumgraphicsview.h \
    Qt/graphicsviewadapterforqt.h \
    Qt/projectmanagerforqt.h \
    Qt/logforqt.h \
    Qt/audioforqt/audioplayerforqt.h \
    Qt/audioforqt/audiorecorderforqt.h \
    Qt/filemanagerforqt.h \
    Qt/initializedialog.h \
    Qt/logviewer.h \
    Qt/recordingstatusgraphicsview.h \
    Qt/settingsforqt.h \
    Qt/options/optionsdialog.h \
    Qt/keyboard/keyboardgraphicsview.h \
    Qt/options/optionscentralwidgetinterface.h \
    Qt/options/optionscontentswidgetinterface.h \
    Qt/options/audio/optionspageaudio.h \
    Qt/options/audio/optionspageaudiomidipage.h \
    Qt/options/audio/optionspageaudioinputoutputpage.h \
    Qt/options/environment/optionspageenvironment.h \
    Qt/options/environment/optionspageenvironmentgeneralpage.h \
    Qt/platformtools.h \
    Qt/donotshowagainmessagebox.h \
    Qt/keyboard/fullscreenkeyboarddialog.h \
    Qt/calculationprogressgroup.h \
    Qt/recordingqualitybar.h \
    Qt/autoclosingmessagebox.h \
    Qt/graphicsitemforqt.h \
    Qt/editpianosheetdialog.h \
    Qt/options/environment/optionspageenvironmenttuningpage.h \
    Qt/doubleslider.h \
    Qt/simplefiledialog.h \
    Qt/algorithmdialog.h \
    Qt/qtconfig.h \
    Qt/keyboard/graphicskeyitem.h \
    Qt/aboutdialog.h \
    Qt/versioncheck.h \
    Qt/runguard.h \
    Qt/displaysize.h \
    Qt/tuninggroupbox.h \
    Qt/displaysizedependinggroupbox.h \
    Qt/signalanalyzergroupbox.h \
    Qt/volumecontrolgroupbox.h \
    Qt/plotsdialog/plotsdialog.h

SOURCES +=  \
    Qt/main.cpp\
    Qt/mainwindow.cpp \
    Qt/volumecontrollevel.cpp \
    Qt/tunerapplication.cpp \
    Qt/fourierspectrumgraph.cpp \
    Qt/tuningcurvegraph.cpp \
    Qt/keyboard/autoscaledtokeyboardgraphicsview.cpp \
    Qt/zoomedspectrumgraphicsview.cpp \
    Qt/graphicsviewadapterforqt.cpp \
    Qt/projectmanagerforqt.cpp \
    Qt/logforqt.cpp \
    Qt/audioforqt/audioplayerforqt.cpp \
    Qt/audioforqt/audiorecorderforqt.cpp \
    Qt/filemanagerforqt.cpp \
    Qt/initializedialog.cpp \
    Qt/logviewer.cpp \
    Qt/recordingstatusgraphicsview.cpp \
    Qt/settingsforqt.cpp \
    Qt/options/optionsdialog.cpp \
    Qt/keyboard/keyboardgraphicsview.cpp \
    Qt/options/optionscentralwidgetinterface.cpp \
    Qt/options/optionscontentswidgetinterface.cpp \
    Qt/options/audio/optionspageaudio.cpp \
    Qt/options/audio/optionspageaudiomidipage.cpp \
    Qt/options/audio/optionspageaudioinputoutputpage.cpp \
    Qt/options/environment/optionspageenvironment.cpp \
    Qt/options/environment/optionspageenvironmentgeneralpage.cpp \
    Qt/platformtools.cpp \
    Qt/donotshowagainmessagebox.cpp \
    Qt/keyboard/fullscreenkeyboarddialog.cpp \
    Qt/calculationprogressgroup.cpp \
    Qt/recordingqualitybar.cpp \
    Qt/autoclosingmessagebox.cpp \
    Qt/graphicsitemforqt.cpp \
    Qt/editpianosheetdialog.cpp \
    Qt/options/environment/optionspageenvironmenttuningpage.cpp \
    Qt/doubleslider.cpp \
    Qt/simplefiledialog.cpp \
    Qt/algorithmdialog.cpp \
    Qt/keyboard/graphicskeyitem.cpp \
    Qt/aboutdialog.cpp \
    Qt/versioncheck.cpp \
    Qt/runguard.cpp \
    Qt/displaysize.cpp \
    Qt/tuninggroupbox.cpp \
    Qt/displaysizedependinggroupbox.cpp \
    Qt/signalanalyzergroupbox.cpp \
    Qt/volumecontrolgroupbox.cpp \
    Qt/plotsdialog/plotsdialog.cpp

#------------- Message system --------------------

CORE_MESSAGE_SYSTEM_HEADERS = \
    core/messages/messagelistener.h \
    core/messages/messagehandler.h \
    core/messages/message.h \
    core/messages/messagerecorderenergychanged.h \
    core/messages/messagemodechanged.h \
    core/messages/messagekeyselectionchanged.h \
    core/messages/messagecaluclationprogress.h \
    core/messages/messageprojectfile.h \
    core/messages/messagemidievent.h \
    core/messages/messagenewfftcalculated.h \
    core/messages/messagepreliminarykey.h \
    core/messages/messagefinalkey.h \
    core/messages/messagechangetuningcurve.h \
    core/messages/messagetuningdeviation.h \
    core/messages/messagekeydatachanged.h \

CORE_MESSAGE_SYSTEM_SOURCES = \
    core/messages/messagelistener.cpp \
    core/messages/messagehandler.cpp \
    core/messages/message.cpp \
    core/messages/messagerecorderenergychanged.cpp \
    core/messages/messagemodechanged.cpp \
    core/messages/messagekeyselectionchanged.cpp \
    core/messages/messagecaluclationprogress.cpp \
    core/messages/messageprojectfile.cpp \
    core/messages/messagemidievent.cpp \
    core/messages/messagenewfftcalculated.cpp \
    core/messages/messagepreliminarykey.cpp \
    core/messages/messagefinalkey.cpp \
    core/messages/messagechangetuningcurve.cpp \
    core/messages/messagetuningdeviation.cpp \
    core/messages/messagekeydatachanged.cpp \

#------------- Drawers --------------------

CORE_DRAWER_HEADERS = \
    core/drawers/drawerbase.h \
    core/drawers/tuningcurvegraphdrawer.h \
    core/drawers/fourierspectrumgraphdrawer.h \
    core/drawers/zoomedspectrumdrawer.h \
    core/drawers/tuningcurvegraphdrawer.h \
    core/drawers/fourierspectrumgraphdrawer.h \
    core/drawers/graphicsitem.h \

CORE_DRAWER_SOURCES = \
    core/drawers/drawerbase.cpp \
    core/drawers/zoomedspectrumdrawer.cpp \
    core/drawers/tuningcurvegraphdrawer.cpp \
    core/drawers/fourierspectrumgraphdrawer.cpp \
    core/drawers/graphicsitem.cpp \

#------------- Adapters --------------------

CORE_ADAPTER_HEADERS = \
    core/adapters/recorderlevel.h \
    core/adapters/modeselectoradapter.h \
    core/adapters/projectmanageradapter.h \
    core/adapters/calculationadapter.h \
    core/adapters/graphicsviewadapter.h \
    core/adapters/filemanager.h \
    core/adapters/coreinitialisationadapter.h \

CORE_ADAPTER_SOURCES = \
    core/adapters/calculationadapter.cpp \
    core/adapters/modeselectoradapter.cpp \
    core/adapters/projectmanageradapter.cpp \
    core/adapters/recorderlevel.cpp \
    core/adapters/filemanager.cpp \
    core/adapters/coreinitialisationadapter.cpp \
    core/adapters/graphicsviewadapter.cpp \

#---------------- Audio -----------------

CORE_AUDIO_HEADERS = \
    core/audio/audiobase.h \
    core/audio/audiorecorderadapter.h \
    core/audio/audioplayeradapter.h \
    core/audio/synthesizer.h \
    core/audio/circularbuffer.h \
    core/audio/soundgenerator.h \
    core/audio/soundgeneratormode.h \
    core/audio/waveformgenerator.h \
    core/audio/hammerknock.h \

CORE_AUDIO_SOURCES = \
    core/audio/audiorecorderadapter.cpp \
    core/audio/audiobase.cpp \
    core/audio/audioplayeradapter.cpp \
    core/audio/synthesizer.cpp \
    core/audio/soundgenerator.cpp \
    core/audio/waveformgenerator.cpp

#----------------- Midi ------------------

CORE_MIDI_HEADERS = \
    core/midi/midiadapter.h \
    core/midi/RtMidiimplementation.h \
    core/midi/NoMidiimplementation.h \

CORE_MIDI_SOURCES = \
    core/midi/midiadapter.cpp \
    core/midi/RtMidiimplementation.cpp \
    core/midi/NoMidiimplementation.cpp \


#------------- Mathematical ----------------

CORE_MATH_HEADERS = \
    core/math/fftadapter.h \
    core/math/fftimplementation.h \
    core/math/mathtools.h \


CORE_MATH_SOURCES = \
    core/math/fftimplementation.cpp \
    core/math/mathtools.cpp \


#--------------- System --------------------

CORE_SYSTEM_HEADERS = \
    core/system/log.h \
    core/system/simplethreadhandler.h \
    core/system/eptexception.h \
    core/system/prerequisites.h \
    core/system/timer.h \
    core/system/version.h \
    core/system/platformtoolscore.h \

CORE_SYSTEM_SOURCES = \
    core/system/log.cpp \
    core/system/simplethreadhandler.cpp \
    core/system/eptexception.cpp \
    core/system/timer.cpp \
    core/system/platformtoolscore.cpp \

#--------------- Analyzers -----------------

CORE_ANALYZER_HEADERS = \
    core/analyzers/signalanalyzer.h \
    core/analyzers/keyrecognizer.h \
    core/analyzers/fftanalyzer.h \
    core/analyzers/fftanalyzererrorcodes.h \

CORE_ANALYZER_SOURCES = \
    core/analyzers/signalanalyzer.cpp \
    core/analyzers/keyrecognizer.cpp \
    core/analyzers/fftanalyzer.cpp \

#---------------- Piano --------------------

CORE_PIANO_HEADERS = \
    core/piano/piano.h \
    core/piano/pianofile.h \
    core/piano/key.h \
    core/piano/pianomanager.h \
    core/piano/keyboard.h \
    core/piano/pianodefines.h \

CORE_PIANO_SOURCES = \
    core/piano/piano.cpp  \
    core/piano/pianofile.cpp \
    core/piano/key.cpp \
    core/piano/pianomanager.cpp \
    core/piano/keyboard.cpp \

#--------------- Calculation ---------------

CORE_CALCULATION_HEADERS = \
    core/calculation/calculationmanager.h \
    core/calculation/algorithm.h \
    core/calculation/algorithmfactory.h \
    core/calculation/algorithmfactorydescription.h \
    core/calculation/algorithminformation.h \
    core/calculation/algorithmparameter.h \
    core/calculation/algorithminformationparser.h \


CORE_CALCULATION_SOURCES = \
    core/calculation/calculationmanager.cpp \
    core/calculation/algorithm.cpp \
    core/calculation/algorithmfactory.cpp \
    core/calculation/algorithmfactorydescription.cpp \
    core/calculation/algorithminformation.cpp \
    core/calculation/algorithmparameter.cpp \
    core/calculation/algorithminformationparser.cpp \

#----------------- Core --------------------

CORE_HEADERS = \
    core/config.h \
    core/core.h \
    core/settings.h \

CORE_SOURCES = \
    core/core.cpp \
    core/settings.cpp \

#---------------- thirdparty ------------------

THIRD_PARTY_HEADERS = \
    thirdparty/RtMidi/RtMidi.h \
    thirdparty/tinyxml2/tinyxml2.h \
    thirdparty/timesupport/timesupport.h \

THIRD_PARTY_SOURCES = \
    thirdparty/RtMidi/RtMidi.cpp \
    thirdparty/tinyxml2/tinyxml2.cpp \
    thirdparty/timesupport/timesupport.cpp \


#---------------- complete --------------------

HEADERS += \
    $$CORE_HEADERS \
    $$CORE_MESSAGE_SYSTEM_HEADERS \
    $$CORE_DRAWER_HEADERS \
    $$CORE_ADAPTER_HEADERS \
    $$CORE_AUDIO_HEADERS \
    $$CORE_MIDI_HEADERS \
    $$CORE_MATH_HEADERS \
    $$CORE_ANALYZER_HEADERS \
    $$CORE_PIANO_HEADERS \
    $$CORE_CALCULATION_HEADERS \
    $$CORE_SYSTEM_HEADERS \
    $$THIRD_PARTY_HEADERS \

SOURCES += \
    $$CORE_SOURCES \
    $$CORE_MESSAGE_SYSTEM_SOURCES \
    $$CORE_DRAWER_SOURCES \
    $$CORE_ADAPTER_SOURCES \
    $$CORE_AUDIO_SOURCES \
    $$CORE_MIDI_SOURCES \
    $$CORE_MATH_SOURCES \
    $$CORE_ANALYZER_SOURCES \
    $$CORE_PIANO_SOURCES \
    $$CORE_CALCULATION_SOURCES \
    $$CORE_SYSTEM_SOURCES \
    $$THIRD_PARTY_SOURCES \


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
win32 {
    include($$PWD/platforms/windows/windows.pri)
}

# add applenativewrapper files
mac {
    HEADERS +=
    OBJECTIVE_SOURCES +=
}

FORMS += \
    Qt/mainwindow.ui \
    Qt/logviewer.ui \
    Qt/editpianosheetdialog.ui

RESOURCES += \
    $$PWD/translations/languages.qrc \
    $$PWD/media/media.qrc \
    $$PWD/tutorial/tutorial.qrc \
    $$PWD/algorithms/algorithms.qrc



# -------- translations -------------

TRANSLATIONS = \
    translations/piano_tuner_de.ts \
    translations/piano_tuner_es.ts \
    translations/piano_tuner_pl.ts \
    translations/piano_tuner_pt.ts \
    translations/piano_tuner_ru.ts \
    translations/piano_tuner_kr.ts \


# -------- algorithms ---------------
algorithmDirs = $$files($$PWD/algorithms/*)
for(algorithmDir, algorithmDirs) {
    algorithmFiles = $$files($$algorithmDir/*)
    for(algorithmFile, algorithmFiles) {
        res = $$find(algorithmFile, pri)
        !isEmpty(res) {
            message(Adding algorithm $$res)
            include($$res)
            !infile($$res, ALGORITHM_SOURCES):error(ALGORITHM_SOURCES not set in $$res)
            !infile($$res, ALGORITHM_HEADERS):error(ALGORITHM_HEADERS not set in $$res)
            for (srcFile, ALGORITHM_SOURCES):SOURCES += $$algorithmDir/$$srcFile
            for (incFile, ALGORITHM_HEADERS):HEADERS += $$algorithmDir/$$incFile
        }
    }
}

# ------------- install ------------
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
