include(../entropypianotuner_config.pri)
include(../entropypianotuner_func.pri)

TEMPLATE = lib

QT += core

contains(EPT_CONFIG, allstatic) {
    CONFIG += staticlib
} else {
    CONFIG += dll
}

CONFIG += c++11

DEFINES += EPT_BUILD_CORE
DESTDIR = $$EPT_CORE_OUT_DIR

INCLUDEPATH += $$EPT_ROOT_DIR $$EPT_BASE_DIR

$$depends_dirent()
$$depends_fftw3()
$$depends_getmemorysize()
$$depends_libuv()
$$depends_rtmidi()
$$depends_timesupport()
$$depends_tinyxml2()


#-------------------------------------------------
#                  Core files
#-------------------------------------------------


#------------- Message system --------------------

CORE_MESSAGE_SYSTEM_HEADERS = \
    messages/messagelistener.h \
    messages/messagehandler.h \
    messages/message.h \
    messages/messagerecorderenergychanged.h \
    messages/messagemodechanged.h \
    messages/messagekeyselectionchanged.h \
    messages/messagecaluclationprogress.h \
    messages/messageprojectfile.h \
    messages/messagemidievent.h \
    messages/messagenewfftcalculated.h \
    messages/messagepreliminarykey.h \
    messages/messagefinalkey.h \
    messages/messagechangetuningcurve.h \
    messages/messagetuningdeviation.h \
    messages/messagekeydatachanged.h \
    messages/messagestroboscope.h \

CORE_MESSAGE_SYSTEM_SOURCES = \
    messages/messagelistener.cpp \
    messages/messagehandler.cpp \
    messages/message.cpp \
    messages/messagerecorderenergychanged.cpp \
    messages/messagemodechanged.cpp \
    messages/messagekeyselectionchanged.cpp \
    messages/messagecaluclationprogress.cpp \
    messages/messageprojectfile.cpp \
    messages/messagemidievent.cpp \
    messages/messagenewfftcalculated.cpp \
    messages/messagepreliminarykey.cpp \
    messages/messagefinalkey.cpp \
    messages/messagechangetuningcurve.cpp \
    messages/messagetuningdeviation.cpp \
    messages/messagekeydatachanged.cpp \
    messages/messagestroboscope.cpp \

#------------- Drawers --------------------

CORE_DRAWER_HEADERS = \
    drawers/drawerbase.h \
    drawers/tuningcurvegraphdrawer.h \
    drawers/fourierspectrumgraphdrawer.h \
    drawers/tuningindicatordrawer.h \
    drawers/tuningcurvegraphdrawer.h \
    drawers/fourierspectrumgraphdrawer.h \
    drawers/graphicsitem.h \

CORE_DRAWER_SOURCES = \
    drawers/drawerbase.cpp \
    drawers/tuningindicatordrawer.cpp \
    drawers/tuningcurvegraphdrawer.cpp \
    drawers/fourierspectrumgraphdrawer.cpp \
    drawers/graphicsitem.cpp \

#------------- Adapters --------------------

CORE_ADAPTER_HEADERS = \
    adapters/recorderlevel.h \
    adapters/modeselectoradapter.h \
    adapters/projectmanageradapter.h \
    adapters/calculationadapter.h \
    adapters/graphicsviewadapter.h \
    adapters/filemanager.h \
    adapters/coreinitialisationadapter.h \

CORE_ADAPTER_SOURCES = \
    adapters/calculationadapter.cpp \
    adapters/modeselectoradapter.cpp \
    adapters/projectmanageradapter.cpp \
    adapters/recorderlevel.cpp \
    adapters/filemanager.cpp \
    adapters/coreinitialisationadapter.cpp \
    adapters/graphicsviewadapter.cpp \

#---------------- Audio -----------------

CORE_AUDIO_HEADERS = \
    audio/audiobase.h \
    audio/circularbuffer.h \
    audio/player/audioplayeradapter.h \
    audio/player/hammerknock.h \
    audio/player/pcmwriterinterface.h \
    audio/player/soundgenerator.h \
    audio/player/synthesizer.h \
    audio/player/waveformgenerator.h \
    audio/midi/midiadapter.h \
    audio/midi/RtMidiimplementation.h \
    audio/midi/NoMidiimplementation.h \
    audio/recorder/audiorecorderadapter.h \
    audio/recorder/stroboscope.h \
    audio/recorder/recordingmanager.h \


CORE_AUDIO_SOURCES = \
    audio/audiobase.cpp \
    audio/player/audioplayeradapter.cpp \
    audio/player/soundgenerator.cpp \
    audio/player/synthesizer.cpp \
    audio/player/waveformgenerator.cpp \
    audio/midi/midiadapter.cpp \
    audio/midi/RtMidiimplementation.cpp \
    audio/midi/NoMidiimplementation.cpp \
    audio/recorder/audiorecorderadapter.cpp \
    audio/recorder/stroboscope.cpp \
    audio/recorder/recordingmanager.cpp \

#------------- Mathematical ----------------

CORE_MATH_HEADERS = \
    math/fftadapter.h \
    math/fftimplementation.h \
    math/mathtools.h \

CORE_MATH_SOURCES = \
    math/fftimplementation.cpp \
    math/mathtools.cpp \

#--------------- System --------------------

CORE_SYSTEM_HEADERS = \
    system/log.h \
    system/simplethreadhandler.h \
    system/eptexception.h \
    system/timer.h \
    system/version.h \
    system/platformtoolscore.h \
    system/serverinfo.h \
    system/basecallback.h \
    system/sharedlibrary.h \

CORE_SYSTEM_SOURCES = \
    system/log.cpp \
    system/simplethreadhandler.cpp \
    system/eptexception.cpp \
    system/timer.cpp \
    system/platformtoolscore.cpp \
    system/serverinfo.cpp \
    system/basecallback.cpp \
    system/sharedlibrary.cpp \

#--------------- Analyzers -----------------

CORE_ANALYZER_HEADERS = \
    analyzers/signalanalyzer.h \
    analyzers/keyrecognizer.h \
    analyzers/fftanalyzer.h \
    analyzers/fftanalyzererrorcodes.h \
    analyzers/overpull.h \

CORE_ANALYZER_SOURCES = \
    analyzers/signalanalyzer.cpp \
    analyzers/keyrecognizer.cpp \
    analyzers/fftanalyzer.cpp \
    analyzers/overpull.cpp \

#---------------- Piano --------------------

CORE_PIANO_HEADERS = \
    piano/piano.h \
    piano/key.h \
    piano/pianomanager.h \
    piano/keyboard.h \
    piano/pianodefines.h \

CORE_PIANO_SOURCES = \
    piano/piano.cpp  \
    piano/key.cpp \
    piano/pianomanager.cpp \
    piano/keyboard.cpp \
    piano/pianodefines.cpp \

#--------------- Calculation ---------------

CORE_CALCULATION_HEADERS = \
    calculation/calculationmanager.h \
    calculation/algorithmplugin.h \
    calculation/algorithm.h \
    calculation/algorithmfactory.h \
    calculation/algorithmfactorydescription.h \
    calculation/algorithminformation.h \
    calculation/algorithmparameterdescription.h \
    calculation/algorithmparameters.h \
    calculation/algorithminformationparser.h \


CORE_CALCULATION_SOURCES = \
    calculation/calculationmanager.cpp \
    calculation/algorithm.cpp \
    calculation/algorithmfactory.cpp \
    calculation/algorithmfactorydescription.cpp \
    calculation/algorithminformation.cpp \
    calculation/algorithmparameterdescription.cpp \
    calculation/algorithmparameters.cpp \
    calculation/algorithminformationparser.cpp \

#----------------- Core --------------------

CORE_HEADERS = \
    config.h \
    core.h \
    settings.h \
    prerequisites.h \

CORE_SOURCES = \
    core.cpp \
    settings.cpp \

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
    $$CORE_SYSTEM_HEADERS

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
    $$CORE_SYSTEM_SOURCES


