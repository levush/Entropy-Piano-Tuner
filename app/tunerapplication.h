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

#ifndef TUNERAPPLICATION_H
#define TUNERAPPLICATION_H

#include <QApplication>

#include "prerequisites.h"

#include "audioforqt/audiorecorderforqt.h"
#include "audioforqt/audioplayerforqt.h"
#include "dialogs/initializedialog.h"
#include "mainwindow/mainwindow.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief The main application singleton class.
///
/// This class handles the initialisation of the MainWindow and the Core.
/// It also stores the instances of the platform dependent audio player and
/// recorder.
/// Moreover this class will progress the messages in MessageHandler.
///////////////////////////////////////////////////////////////////////////////
class TunerApplication : public QApplication
{
    Q_OBJECT
public:
    /// The one and only instance.
    static TunerApplication *mSingleton;
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Constructor for the application.
    /// \param argc : Number of arguments
    /// \param argv : Array of the arguments
    ///
    /// The constructor will create the MainWindow and the Core without
    /// initializing them.
    /// It will also call the platform dependent function to disable the
    /// screen saver.
    ///////////////////////////////////////////////////////////////////////////////
    explicit TunerApplication(int & argc, char ** argv);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Destructor of the application.
    ///
    /// The destructor makes sure that all components are exitted and enables the
    /// screen saver.
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~TunerApplication();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter for the singleton instance.
    /// \return mSingleton
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static TunerApplication &getSingleton();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter for the singleton instance.
    /// \return Pointer to mSingleton or nullptr if not created yet.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static TunerApplication *getSingletonPtr();


    /// \brief This is sets the exit code if the app would be terminated now
    ///
    /// If it is expected that the app may exit in the future, usually at the end
    /// of the program all this with EXIT_SUCCESS.
    /// If it is not expected that the app exits call this using EXIT_FAILURE
    /// This is used to detect whether the app crashed
    static void setApplicationExitState(int errorcode = EXIT_SUCCESS);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to initialise the application.
    ///
    /// This will initialize the MainWindow and show it.
    /// Secondly the Core will be initialized.
    ///////////////////////////////////////////////////////////////////////////////
    void init();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function called upon exitting the application.
    ///
    /// This will stop and exit the core.
    ///////////////////////////////////////////////////////////////////////////////
    void exit();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to start the MainWindow and the Core.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void start();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to stop the MainWindow and the Core.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void stop();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to play the startup sound.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void playStartupSound();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Getter function for the core.
    /// \return Pointer stored in mCore
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Core *getCore() {return mCore.get();}

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Handling of general events.
    /// \param e : The QEvent
    /// \return If the event was processed.
    ///
    /// On MacOS X this function will listen to QEvent::FileOpen to open a startup
    /// file.
    ///////////////////////////////////////////////////////////////////////////////
    bool event(QEvent *e);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Called when the internal timer was shot.
    /// \param event : The QTimerEvent
    ///
    /// This function will progress the messages in MessageHandler.
    ///////////////////////////////////////////////////////////////////////////////
    void timerEvent(QTimerEvent *event);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Reimplemented to catch exceptions.
    /// \param receiver : The receiving QObject.
    /// \param event : The QEvent
    /// \return The result of QApplication::notify(QObject*, QEvent*)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool notify(QObject* receiver, QEvent* event);

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Initialising of the core.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void initCore();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Exit from the core.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void exitCore();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Start the core.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void startCore();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Stop the core.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void stopCore();

public slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Open the given file.
    /// \param filePath : The absolute file path
    /// \param cached : Is the file stored in cache
    /// \return Result of the open command
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool openFile(QString filePath, bool cached);

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Depending on the application state the core will be started or
    ///        stopped.
    /// \param state : The Qt::ApplicationState
    ///
    /// When minimized the core will stop. On mobile devices the core will already
    /// stop if the application is in inavtive state.
    ///////////////////////////////////////////////////////////////////////////////
    void onApplicationStateChanged(Qt::ApplicationState state);

    void onAboutToQuit();

private:
    /// last exit code to detect if the application crashed
    int mLastExitCode;

    /// Id of the timer that progresses the MessageHandler.
    int mMessageHandlerTimerId;

    /// Absolute path to the startup file or an empty string.
    QString mStartupFile;

    /// Shared pointer of the Core.
    std::shared_ptr<Core> mCore;

    /// Shared pointer of the MainWindow.
    std::shared_ptr<MainWindow> mMainWindow;

    /// Instance of the Qt audio recorder.
    AudioRecorderForQt mAudioRecorder;

    /// Instance of the Qt audio player.
    AudioPlayerForQt mAudioPlayer;
};

#endif // TUNERAPPLICATION_H
