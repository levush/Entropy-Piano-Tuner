/*****************************************************************************
 * Copyright 2016 Haye Hinrichsen, Christoph Wick
 *
 * This file is part of Entropy Piano Tuner.
 *
 * Entropy Piano Tuner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Entropy Piano Tuner is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Entropy Piano Tuner. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#include "tunerapplication.h"

#include <iostream>
#include <QDebug>
#include <QThread>
#include <QMediaPlayer>
#include <QFile>
#include <QResource>
#include <QFileOpenEvent>
#include <QStandardPaths>
#include <QScreen>
#include <QMessageBox>

#include "core/messages/messagehandler.h"
#include "core/system/eptexception.h"
#include "core/config.h"

#include "dialogs/log/logviewer.h"
#include "implementations/platformtools.h"
#include "implementations/projectmanagerforqt.h"
#include "implementations/settingsforqt.h"

TunerApplication *TunerApplication::mSingleton(nullptr);

TunerApplication::TunerApplication(int & argc, char ** argv)
    : QApplication(argc, argv),
      mMessageHandlerTimerId(0),
      mAudioRecorder(this),
      mAudioPlayer(this) {

    EptAssert(!mSingleton, "Singleton class already created");
    mSingleton = this;
}

TunerApplication::~TunerApplication()
{
    stop();
    exit();
    mCore.reset();

    mSingleton = nullptr;
}

TunerApplication &TunerApplication::getSingleton() {
    EptAssert(mSingleton, "Class has to be created");
    return *mSingleton;
}

TunerApplication *TunerApplication::getSingletonPtr() {
    return mSingleton;
}

void TunerApplication::setApplicationExitState(int errorcode) {
    // store the error code for next session
    QSettings settings;
    settings.setValue("application/lastExitCode", errorcode);
}

void TunerApplication::init() {
    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << ":/media" << ":/media/icons");

    if (primaryScreen()->devicePixelRatio() >= 1.5) {
        setAttribute(Qt::AA_UseHighDpiPixmaps);
    }


    // open the main window with the startup file
    mMainWindow.reset(new MainWindow());
#ifdef Q_OS_MOBILE
    // fix fullscreen size on mobile devices
    mMainWindow->setFixedSize(primaryScreen()->size());
#endif

    // get last exit code
    QSettings settings;
    mLastExitCode =  settings.value("application/lastExitCode", EXIT_SUCCESS).toInt();
    // and set the last exit code to failure (this session has not ended)
    settings.setValue("application/lastExitCode", EXIT_FAILURE);

    QObject::connect(this, SIGNAL(aboutToQuit()), this, SLOT(onAboutToQuit()));

    // check if there was a crash last session
    if (mLastExitCode != EXIT_SUCCESS) {
        QMainWindow *m = mMainWindow.get();
        if (QMessageBox::information(m, tr("Crash handler"), tr("The application exited unexpectedly on the last run. Do you want to view the last log?"), QMessageBox::Yes | QMessageBox::No)
                == QMessageBox::Yes) {
            LogViewer v(LogViewer::PREVIOUS_LOG, m);
            v.exec();
        }
    }

    // writeout args to log
    LogI("Number of arguments: %d", arguments().size());
    LogI("Program arguments: %s", arguments().join(", ").toStdString().c_str());

    // create core
    mCore.reset(new Core(
                    new ProjectManagerForQt(mMainWindow.get()),
                    &mAudioRecorder,
                    &mAudioPlayer));

    // print memory information, the waveform generator will check cases
    // to determine the length of the waveforms
    double physicalMemoryInGiB = PlatformTools::getSingleton()->getInstalledPhysicalMemoryInB() / 1024.0 / 1024.0 / 1024.0;
    LogI("Installed Physical memory: %f GiB", physicalMemoryInGiB);

    PlatformTools::getSingleton()->disableScreensaver();


    EptAssert(mCore, "Core has to be created before entering init");

    // init the window
    LogI("Initializing the main window");
    mMainWindow->init(mCore.get());

    // init platform components
    LogI("Initializing the platform tools");
    PlatformTools::getSingleton()->init();

    // then init the core
    LogI("Initializing the core");
    initCore();

    LogI("Initialized");
}

void TunerApplication::exit() {
    stop();

    if (!mCore) {return;}
    mCore->exit();
}

void TunerApplication::start() {
    MessageHandler::getSingleton().process();
    startCore();

    mMainWindow->start();

    QObject::connect(this, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
                     this, SLOT(onApplicationStateChanged(Qt::ApplicationState)));


    // if the user opened this program by double clicking a file, we can set this file
    // as startup file, that is loaded when the program has started.
    // if startupFile is empty, we open the empty default file
    if (mStartupFile.size() > 0) {
        openFile(mStartupFile, false);
        mStartupFile.clear();
    } else {
        PlatformTools::getSingleton()->loadStartupFile(arguments());
    }
}

void TunerApplication::stop() {
    if (!mCore) {return;}
    mCore->stop();
}

void TunerApplication::playStartupSound() {
    // play a startup sound
#ifdef __linux__
    QString fileName="startup_sound.ogg";
#else
    QString fileName="startup_sound.mp3";
#endif

    // first copy startup sound from resource location to disk
    // it cannot be played out a resource file
    QFile audioFile(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/" + fileName);
    if (!audioFile.exists()) {
        // file does not exists yet, copy it
        QFile::copy(":/media/audio/" + fileName, audioFile.fileName());
    } else {
        if (audioFile.size() != QResource(":/media/audio/" + fileName).size()) {
            // size changed, this usually means a new size, remove and copy
            audioFile.remove();
            QFile::copy(":/media/audio/" + fileName, audioFile.fileName());
        }
    }

    // file should exist now
    EptAssert(audioFile.exists(), "Audio file should exist now");

    // play the actual sound
    QMediaPlayer *player = new QMediaPlayer(this);
    player->setMedia(QUrl::fromLocalFile(audioFile.fileName()));
    player->setVolume(50);
    player->play();
    if (player->error() != QMediaPlayer::NoError) {
        LogW("Error in QMediaPlayer: %s", player->errorString().toStdString().c_str());
    }
}

bool TunerApplication::openFile(QString filePath, bool cached) {
    if (!mCore || !mCore->getProjectManager() || !mCore->isInitialized()) {
        // not initiated, save file for later use
        LogI("Storing startup file: %s", filePath.toStdString().c_str());
        mStartupFile = filePath;
        return true;
    }
    return mCore->getProjectManager()->openFile(filePath.toStdWString(), cached) == ProjectManagerAdapter::R_ACCEPTED;
}

bool TunerApplication::event(QEvent *e) {
    switch (e->type()) {
    case QEvent::FileOpen:
        // note: this only supported in Mac OS X so far!
        return openFile(static_cast<QFileOpenEvent *>(e)->file(), false);
    default:
        return QApplication::event(e);
    }
}

void TunerApplication::timerEvent(QTimerEvent *event) {
    MessageHandler::getSingleton().process();
    (void)event; // event not used, suppress warning
}

bool TunerApplication::notify(QObject* receiver, QEvent* event) {
    try {
        return QApplication::notify(receiver, event);
    }
    catch (const EptException &e) {
        qCritical() << "Unhandled exception: ";
        qCritical() << QString::fromStdString(e.getFullDescription());

    }
    catch (const std::exception &e) {
        qCritical() << "Unhandled exception: ";
        qCritical() << QString::fromStdString(e.what());
    }
    catch (...) {
        qCritical() << "Unhandled exception: ";
        qCritical() << "unknown exception";
    }
  return true;
}

void TunerApplication::initCore() {
    if (mCore && !mCore->isInitialized()) {
        // disable main window during init
        mMainWindow->setEnabled(false);

        mCore->init(new QtCoreInitialisation(mMainWindow.get()));

        // enable the main dialog again
        mMainWindow->setEnabled(true);

        // recativate window after closing the dialog
        mMainWindow->activateWindow();
    }
}

void TunerApplication::exitCore() {
    if (mCore && mCore->isInitialized()) {
        mCore->exit();
    }

}

void TunerApplication::startCore() {
    if (mCore) {
        mCore->start();
    }


    // custom message loop
    mMessageHandlerTimerId = startTimer(10);
}

void TunerApplication::stopCore() {
    if (mCore) {
        mCore->stop();
    }

    // kill the custom timer
    if (mMessageHandlerTimerId) {
        killTimer(mMessageHandlerTimerId);
        mMessageHandlerTimerId = 0;
    }
}

void TunerApplication::onApplicationStateChanged(Qt::ApplicationState state) {
    if (state & Qt::ApplicationSuspended) {
        // called if application is 'shut down'
        LogI("Application suspended: exiting core");
        setApplicationExitState(EXIT_SUCCESS);
        stopCore();
        exitCore();
    } else if (state & Qt::ApplicationActive) {
        // init and start core components
        LogI("Application gone active: starting core");
        setApplicationExitState(EXIT_FAILURE);
        initCore();
        startCore();
    } else if (state & Qt::ApplicationHidden ) {
        // delete core components
        LogI("Application gone hidden: exiting core");
        exitCore();
    } else if (state & Qt::ApplicationInactive) {
        // stop the core on mobile platforms
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(Q_OS_WINPHONE)
        LogI("Application gone inactive: stopping core");
        stopCore();
        setApplicationExitState(EXIT_SUCCESS);
#endif
    }
}

void TunerApplication::onAboutToQuit() {
    setApplicationExitState(EXIT_SUCCESS);
}
