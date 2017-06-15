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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../core/messages/message.h"
#include "../core/messages/messagehandler.h"
#include "../core/messages/messagemodechanged.h"
#include "../core/messages/messageprojectfile.h"
#include "../core/messages/messagekeyselectionchanged.h"
#include "../core/messages/messagerecorderenergychanged.h"
#include "../core/messages/messagefinalkey.h"
#include "../core/messages/messagecaluclationprogress.h"
#include "../core/messages/messagenewfftcalculated.h"
#include "../core/messages/messagesignalanalysis.h"
#include "../core/system/version.h"
#include "../core/system/serverinfo.h"
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QToolButton>
#include <QStatusBar>
#include <QButtonGroup>
#include <QWhatsThis>
#include <QShortcut>
#include <QGuiApplication>
#include <QScreen>
#include <QCheckBox>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QTextEdit>
#include <QTextBrowser>
#include <QResource>
#include <QDialogButtonBox>
#include <QScroller>
#ifndef QT_NO_PROCESS
#include <QProcess>
#endif

#include "dialogs/aboutdialog.h"
#include "dialogs/autoclosingmessagebox.h"
#include "dialogs/donotshowagainmessagebox.h"
#include "dialogs/editpianosheet/editpianosheetdialog.h"
#include "dialogs/log/logviewer.h"
#include "dialogs/plotsdialog/plotsdialog.h"
#include "implementations/settingsforqt.h"
#include "options/optionsdialog.h"

#include "qtconfig.h"
#include "displaysize.h"

const int MAX_NUMBER_OF_INVALID_RECORDINGS_BEFORE_USER_INFORMATION = 3;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mIconPostfix((QApplication::primaryScreen()->devicePixelRatio() > 1.5) ? "@2x" : ""),
    mCore(nullptr),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphSplitter->setSizes(QList<int>() << 200 << 200);

    // this is in mm
    qreal maxDisplaySize = std::max(QGuiApplication::primaryScreen()->physicalSize().width(),
                                    QGuiApplication::primaryScreen()->physicalSize().height());
    new DisplaySizeDefines(maxDisplaySize * 0.0393701, ui->modeToolBar->fontMetrics().height());  // convert mm to inch
    // switch for testing a display size
    // new DisplaySizeDefines(DS_XSMALL);

    mVolumeControlGroup = new VolumeControlGroupBox(this);
    ui->controlLayout->addWidget(mVolumeControlGroup, 1);
    QObject::connect(this, SIGNAL(modeChanged(OperationMode)), mVolumeControlGroup, SLOT(onModeChanged(OperationMode)));
    QObject::connect(mVolumeControlGroup, SIGNAL(refreshInputLevels()), this, SLOT(onResetNoiseLevel()));
    QObject::connect(mVolumeControlGroup, SIGNAL(muteMicroToggled(bool)), this, SLOT(onToggleMicroMute(bool)));
    QObject::connect(mVolumeControlGroup, SIGNAL(muteSpeakerToggled(bool)), this, SLOT(onToggleSpeakerMute(bool)));

    mSignalAnalyzerGroup = new SignalAnalyzerGroupBox(this);
    ui->controlLayout->addWidget(mSignalAnalyzerGroup, 0);
    QObject::connect(this, SIGNAL(modeChanged(OperationMode)), mSignalAnalyzerGroup, SLOT(onModeChanged(OperationMode)));

    mTuningIndicatorGroup = new TuningIndicatorGroupBox(this);
    ui->controlLayout->addWidget(mTuningIndicatorGroup, 1);
    QObject::connect(this, SIGNAL(modeChanged(OperationMode)), mTuningIndicatorGroup, SLOT(onModeChanged(OperationMode)));

    mKeyboardGraphicsView = new KeyboardGraphicsView(this);
    qobject_cast<QBoxLayout*>(ui->centralWidget->layout())->addWidget(mKeyboardGraphicsView);


    // Tool bars
    int iconSize = ui->modeToolBar->fontMetrics().height();
    QSize modeIconSize = QSize(iconSize, iconSize) * 2;
    ui->modeToolBar->setObjectName("modeToolBar");
    ui->modeToolBar->setIconSize(QSize(iconSize, iconSize) * 2);


    QScrollArea *modeScrollArea = new QScrollArea;
    ui->modeToolBar->addWidget(modeScrollArea);
    ui->modeToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    modeScrollArea->setWidgetResizable(true);
    modeScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    modeScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    modeScrollArea->setStyleSheet("\
                        QScrollArea { background: transparent; } \
                        QScrollArea > QWidget > QWidget { background: transparent; } \
                        ");
    modeScrollArea->setFrameShape(QFrame::NoFrame);
    QScroller::grabGesture(modeScrollArea->viewport(), QScroller::LeftMouseButtonGesture);
    QWidget *modeScrollContents = new QWidget;
    modeScrollArea->setWidget(modeScrollContents);
    modeScrollContents->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QVBoxLayout *modeScrollLayout = new QVBoxLayout;
    modeScrollLayout->setMargin(0);
    modeScrollLayout->setSpacing(0);
    modeScrollContents->setLayout(modeScrollLayout);

    QButtonGroup *group = new QButtonGroup(this);

    auto createModeToolButton = [this, modeIconSize, group, modeScrollLayout]() {
        QToolButton *b = new QToolButton(this);
        group->addButton(b);
        b->setAutoRaise(true);
        b->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        b->setCheckable(true);
        b->setIconSize(modeIconSize);
        b->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

        modeScrollLayout->addWidget(b);
        return b;
    };

    QToolButton *idleButton = createModeToolButton();
    idleButton->setText(tr("Idle"));
    idleButton->setIcon(QIcon(":/media/icons/system-idle.png"));
    idleButton->setWhatsThis(tr("Press this button to switch to the idle mode."));
    connect(idleButton, SIGNAL(toggled(bool)), this, SLOT(onModeIdle(bool)));

    QToolButton *recordingButton = createModeToolButton();
    recordingButton->setText(tr("Record"));
    recordingButton->setIcon(QIcon::fromTheme("audio-input-microphone", QIcon(":/media/icons/audio-input-microphone.png")));
    recordingButton->setWhatsThis(tr("Press this button to switch to the recording mode."));
    connect(recordingButton, SIGNAL(toggled(bool)), this, SLOT(onModeRecord(bool)));

    QToolButton *calculatingButton = createModeToolButton();
    calculatingButton->setText(tr("Calculate"));
    calculatingButton->setIcon(QIcon(":/media/icons/note.png"));
    calculatingButton->setWhatsThis(tr("Press this button to switch to the calculation mode."));
    connect(calculatingButton, SIGNAL(toggled(bool)), this, SLOT(onModeCalculate(bool)));

    QToolButton *tuningButton = createModeToolButton();
    tuningButton->setText(tr("Tune"));
    tuningButton->setIcon(QIcon(":/media/icons/wrest.png"));
    tuningButton->setWhatsThis(tr("Press this button to switch to the tuning mode."));
    connect(tuningButton, SIGNAL(toggled(bool)), this, SLOT(onModeTune(bool)));

    idleButton->setChecked(true);

    modeScrollLayout->addStretch();

    mModeToolButtons[MODE_IDLE] = idleButton;
    mModeToolButtons[MODE_RECORDING] = recordingButton;
    mModeToolButtons[MODE_CALCULATION] = calculatingButton;
    mModeToolButtons[MODE_TUNING] = tuningButton;

    // helper function to get default icon from system or from local icon theme
    auto iconFromTheme = [](const QString &s)  {return QIcon::fromTheme(s, QIcon(":/media/icons/" + s + ".png"));};

    // file tool bar, containing most relevant buttons
    mFileToolBar = new QToolBar(tr("Documents and tools"));
    mFileToolBar->setObjectName("fileToolBar");
    addToolBar(Qt::TopToolBarArea, mFileToolBar);
    mFileToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::LeftToolBarArea);
    mFileToolBar->setMovable(false);
    mFileToolBar->setIconSize(QSize(iconSize, iconSize) * 1.5);
    mFileToolBar->addAction(iconFromTheme("document-new"), tr("New"), this, SLOT(onFileNew()));
    mFileToolBar->addAction(iconFromTheme("document-open"), tr("Open"), this, SLOT(onFileOpen()));
    mFileToolBar->addAction(iconFromTheme("document-save"), tr("Save"), this, SLOT(onFileSave()));
    mFileToolBar->addAction(iconFromTheme("document-save-as"), tr("Save as"), this, SLOT(onFileSaveAs()));
    mFileToolBar->addAction(iconFromTheme("edit-piano-sheet"), tr("Edit piano data sheet"), this, SLOT(onEditPianoDataSheet()));
    mFileToolBar->addAction(iconFromTheme("edit-clear"), tr("Clear pitch markers"), this, SLOT(onResetRecording()));
    mFileToolBar->addSeparator();
    mFileToolBar->addAction(iconFromTheme("preferences-system"), tr("Options"), this, SLOT(onOptions()));
    mFileToolBar->addAction(QIcon(":/media/icons/mathematical_plot.png"), tr("Graphs"), this, SLOT(onOpenPlots()));
    mFileToolBar->setContextMenuPolicy(Qt::PreventContextMenu);

    QToolBar *helpToolBar = new QToolBar(tr("Help"));
    helpToolBar->setObjectName("helpToolBar");
    addToolBar(Qt::TopToolBarArea, helpToolBar);
    helpToolBar->setMovable(false);
    helpToolBar->setFloatable(false);
    helpToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::LeftToolBarArea);
    helpToolBar->setIconSize(mFileToolBar->iconSize());
    helpToolBar->setContextMenuPolicy(Qt::PreventContextMenu);

    QWidget *tbStretch = new QWidget;
    helpToolBar->addWidget(tbStretch);
    tbStretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    tbStretch->setVisible(true);

    helpToolBar->addAction(iconFromTheme("help-contents"), tr("Tutorial"), this, SLOT(onTutorial()));
    //helpToolBar->addAction(iconFromTheme("accessories-text-editor"), tr("Log"), this, SLOT(onViewLog()));
    helpToolBar->addAction(iconFromTheme("help-about"), tr("About"), this, SLOT(onAbout()));


    // main menu icons
#if CONFIG_MENU_ICONS
    ui->action_New->setIcon(iconFromTheme("document-new"));
    ui->actionOpen->setIcon(iconFromTheme("document-open"));
    ui->actionSave->setIcon(iconFromTheme("document-save"));
    ui->actionSave_As->setIcon(iconFromTheme("document-save-as"));
    ui->actionExit->setIcon(iconFromTheme("application-exit"));
    ui->actionEdit_piano_data_sheet->setIcon(iconFromTheme("edit-piano-sheet"));
    ui->actionClear_pitches->setIcon(iconFromTheme("edit-clear"));
    ui->actionOptions->setIcon(iconFromTheme("preferences-system"));
    ui->actionTutorial->setIcon(iconFromTheme("help-contents"));
    ui->actionView_log->setIcon(iconFromTheme("accessories-text-editor"));
    ui->actionAbout->setIcon(iconFromTheme("help-about"));
#endif

#ifdef Q_OS_DESKTOP
    // enable export button
    QAction *exportAction = new QAction(tr("Export"), ui->menuFile);
    QObject::connect(exportAction, SIGNAL(triggered(bool)), this, SLOT(onExport()));
    ui->menuFile->insertAction(ui->actionShare, exportAction);
    ui->menuFile->insertSeparator(exportAction);
#endif

#if __APPLE__
#   if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
    ui->menuBar->setVisible(false);
#   endif
#endif

#ifdef __ANDROID__
    // show share button on android
    ui->actionShare->setVisible(true);
#endif  // __ANDROID__

#if EPT_WHATS_THIS_ENABLED
    QAction *whatsThisAction = QWhatsThis::createAction(this);
    whatsThisAction->setIcon(QIcon(":/media/icons/help-context.png"));
    whatsThisAction->setCheckable(true);
    ui->menuHelp->addAction(whatsThisAction);
    helpToolBar->addAction(whatsThisAction);
#endif  // EPT_WHATS_THIS_ENABLED

    // create shortcuts
    new QShortcut(QKeySequence(Qt::Key_Tab), this, SLOT(onSelectNextMode()));
    new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_Tab), this, SLOT(onSelectPreviousMode()));
    new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(onToggleFullscreen()));
    new QShortcut(QKeySequence(Qt::Key_F), this, SLOT(onToggleFullscreen()));
    new QShortcut(QKeySequence::FullScreen, this, SLOT(onToggleFullscreen()));
    new QShortcut(QKeySequence(Qt::Key_Space), this, SLOT(onToggleTuningIndictator()));

    ui->action_New->setShortcut(QKeySequence::New);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSave_As->setShortcut(QKeySequence::SaveAs);
    ui->actionExit->setShortcut(QKeySequence::Quit);
    ui->actionOptions->setShortcut(QKeySequence::Preferences);
    ui->actionTutorial->setShortcut(QKeySequence::HelpContents);
    ui->actionEdit_piano_data_sheet->setShortcut(QKeySequence(Qt::Key_F9));

    ui->fourierSpectrumGraphics->setKeyboard(mKeyboardGraphicsView);
    ui->tuningCurveGraphicsView->setKeyboard(mKeyboardGraphicsView);


    QBoxLayout *mainLayout = qobject_cast<QBoxLayout*>(centralWidget()->layout());
    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 1);
    mainLayout->setStretch(3, 1);

    mProgressDisplay = new ProgressDisplay(this);

#if CONFIG_DIALOG_SIZE == 1
    // reset old settings (windows size/position and splitter)
    QSettings settings;
    restoreGeometry(settings.value("geometry/mainwindow").toByteArray());
    restoreState(settings.value("windowState/mainwindow").toByteArray());
    QList<int> sizes;
    for (QVariant v : settings.value("splitterSizes/graphSplitter").toList()) {
        sizes.push_back(v.toInt());
    }
    if (sizes.size() > 0) {
        ui->graphSplitter->setSizes(sizes);
    }
    show();
#else
    SHOW_DIALOG(this);
#endif  // CONFIG_DIALOG_SIZE
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init(Core *core) {
    EptAssert(core, "Core is required");
    mCore = core;

    LogV("Creating QMidiAutoConnector");
    mMidiAutoConnector = new QMidiAutoConnector(this);
    connect(mMidiAutoConnector, &QMidiAutoConnector::inputDeviceCreated, this, &MainWindow::onMidiInputDeviceCreated);

    qDebug() << "Display size: " << QGuiApplication::primaryScreen()->physicalSize();

    mCore->getProjectManager()->setCallback(this);

    // hide some elements
    if (DisplaySizeDefines::getSingleton()->getGraphDisplayMode() == GDM_ONE_VISIBLE) {
        ui->fourierSpectrumGraphics->setVisible(false);          // only visible during recording

        // hide status bar
        statusBar()->setHidden(true);
    }

    ui->controlLayout->addWidget(mCalculationProgressGroup = new CalculationProgressGroup(mCore, this));
    mCalculationProgressGroup->setVisible(false);  // not visible in idle mode
}

void MainWindow::start() {
    updateVolumeBar();

    // init the project manager (load startup file)
    mCore->getProjectManager()->init(mCore);

    // add progress display as listener of the WaveformGenerator
    if (mCore->getSoundGenerator()) {
        mCore->getSoundGenerator()->getSynthesizer().getWaveformGenerator().addListener(mProgressDisplay);
    }

    updateWindowTitle();

    // check for updates
    VersionCheck *versionChecker = new VersionCheck(this);
    QObject::connect(versionChecker, SIGNAL(updateAvailable(VersionInformation)), this, SLOT(onVersionUpdate(VersionInformation)));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (mCore->getProjectManager()->onQuit()) {
        mClosing = true;
#if CONFIG_DIALOG_SIZE == 1
        QSettings settings;
        settings.setValue("geometry/mainwindow", saveGeometry());
        settings.setValue("windowState/mainwindow", saveState());
        QVariantList sizes;
        for (int s : ui->graphSplitter->sizes()) {
            sizes.push_back(QVariant(s));
        }
        settings.setValue("splitterSizes/graphSplitter", sizes);
#endif

        event->accept();
        QMainWindow::closeEvent(event);
    } else {
        event->ignore();
    }
}

void MainWindow::handleMessage(MessagePtr m) {
    switch (m->getType()) {
    case Message::MSG_PROJECT_FILE: {
        auto mpf(std::static_pointer_cast<MessageProjectFile>(m));
        switch (mpf->getFileMessageType()) {
        case MessageProjectFile::FILE_CREATED:
            statusBar()->showMessage(tr("File created"));
            break;
        case MessageProjectFile::FILE_EDITED:
            statusBar()->showMessage(tr("File edited"));
            break;
        case MessageProjectFile::FILE_OPENED:
            statusBar()->showMessage(tr("File opened"));
            break;
        case MessageProjectFile::FILE_SAVED:
            statusBar()->showMessage(tr("File saved"));
            break;
        default:
            LogW("MessagePojectFile::Type Not implemented");
            break;
        }

        break;
    }
    case Message::MSG_MODE_CHANGED: {
        auto mmc(std::static_pointer_cast<MessageModeChanged>(m));

        // check the correct tool button
        mModeToolButtons[mmc->getMode()]->setChecked(true);

        emit modeChanged(mmc->getMode());

        // hide all groups first and enable if required
        if (mCalculationProgressGroup) {
            mCalculationProgressGroup->setVisible(false);
        }

        // ask user if he wants to save
        if (mCore && mCore->getProjectManager()->hasChangesInFile()) {
            if (DoNotShowAgainMessageBox::show(DoNotShowAgainMessageBox::MODE_CHANGE_SAVE, this) == QMessageBox::Yes) {
                // save
                if (mCore->getProjectManager()->onSaveFile() == ProjectManagerAdapter::R_CANCELED) {
                    // if the user cancels the dialog, reset the save state of the dialog
                    SettingsForQt::getSingleton().setDoNotShowAgainMessageBox(DoNotShowAgainMessageBox::MODE_CHANGE_SAVE, false, -1);
                }
            }
        }

        // display
        if (mmc->getMode() == MODE_CALCULATION) {
            // hide first for correct sizing
            mCalculationProgressGroup->setVisible(true);
            if (DisplaySizeDefines::getSingleton()->getGraphDisplayMode() == GDM_ONE_VISIBLE) {
                ui->fourierSpectrumGraphics->setVisible(false);
            }
        } else {
            // hide first for correct sizing
            if (DisplaySizeDefines::getSingleton()->getGraphDisplayMode() != GDM_ONE_VISIBLE) {
                ui->fourierSpectrumGraphics->setVisible(true);
            }
        }

        if (mmc->getMode() == MODE_TUNING) {

            // perform a small check if tuning is possible
            int missingKeyRecordings = 0;
            int missingComputedFrequencies = 0;

            for (const Key &key : mCore->getPianoManager()->getPiano().getKeyboard().getKeys()) {
                if (!key.isRecorded()) {
                    missingKeyRecordings++;
                }
                if (key.getComputedFrequency() <= 0) {
                    missingComputedFrequencies++;
                }
            }

            // print the result of the check
            if (missingKeyRecordings > 0 && !DoNotShowAgainMessageBox::doNotShowAgain(DoNotShowAgainMessageBox::NOT_ALL_KEYS_RECORDED)) {
                // manually check if reshow, that we reach other else and if's
                // we do not want to show more than one box at a time
                DoNotShowAgainMessageBox::show(DoNotShowAgainMessageBox::NOT_ALL_KEYS_RECORDED, this);
            }
            else if (missingComputedFrequencies == mCore->getPianoManager()->getPiano().getKeyboard().getNumberOfKeys()) {
                DoNotShowAgainMessageBox::show(DoNotShowAgainMessageBox::TUNING_CURVE_NOT_CALCULATED, this);
            } else if (missingComputedFrequencies > 0) {
                DoNotShowAgainMessageBox::show(DoNotShowAgainMessageBox::TUNING_CURVE_NEEDS_UPDATE, this);
            }
        } else  {
        }

        updateFrequency(mKeyboardGraphicsView->getSelectedKey());
        break;
    }
    case Message::MSG_RECORDING_STARTED:
        statusBar()->showMessage(tr("Recording keystroke"));
        if (DisplaySizeDefines::getSingleton()->getGraphDisplayMode() == GDM_ONE_VISIBLE) {
            ui->tuningCurveGraphicsView->setVisible(false);
            ui->fourierSpectrumGraphics->setVisible(true);
        }
        mSignalAnalyzerGroup->setFrequency("-");
        break;
    case Message::MSG_SIGNAL_ANALYSIS: {
        auto msa(std::static_pointer_cast<MessageSignalAnalysis>(m));
        if (msa->status() == MessageSignalAnalysis::Status::STARTED) {
            statusBar()->showMessage(tr("Signal analysis started"));
        } else {
            if (DisplaySizeDefines::getSingleton()->getGraphDisplayMode() == GDM_ONE_VISIBLE) {
                ui->fourierSpectrumGraphics->setVisible(false);
                ui->tuningCurveGraphicsView->setVisible(true);
            }
            switch (msa->result()) {
            case MessageSignalAnalysis::Result::SUCCESSFULL:
                statusBar()->showMessage(tr("Signal analysis ended"));
                break;
            case MessageSignalAnalysis::Result::INVALID:
                statusBar()->showMessage(tr("Signal analysis failed"));
                if (!mClosing && mCurrentMode == MODE_RECORDING) {
                    if (msa->invalidAttempts() >= MAX_NUMBER_OF_INVALID_RECORDINGS_BEFORE_USER_INFORMATION) {
                        LogD("Maximum number of invalid recordings reached. Trying to inform the user.");
                        DoNotShowAgainMessageBox::show(DoNotShowAgainMessageBox::FORCE_RECORDING_INFORMATION, this);
                    }
                }
            }
        }
        break;
    }
    case Message::MSG_KEY_SELECTION_CHANGED: {
        auto mksc(std::static_pointer_cast<MessageKeySelectionChanged>(m));
        updateNoteName(mksc->getKeyNumber());
        updateFrequency(mksc->getKey());
        break;}
    case Message::MSG_RECORDER_ENERGY_CHANGED: {
        auto mrec(std::static_pointer_cast<MessageRecorderEnergyChanged>(m));
        if (mrec->getLevelType() == MessageRecorderEnergyChanged::LevelType::LEVEL_OFF) {
            // off level changed, adjust the volume bar indicator positions
            updateVolumeBar();
        }
        break;
    }
    case Message::MSG_FINAL_KEY: {
        auto message(std::static_pointer_cast<MessageFinalKey>(m));
        auto keyptr = message->getFinalKey(); // get shared pointer to the new key
        updateFrequency(keyptr.get());
        break;
    }
    case Message::MSG_CALCULATION_PROGRESS: {
        auto mcp(std::static_pointer_cast<MessageCaluclationProgress>(m));
        switch (mcp->getCalculationType()) {
        case MessageCaluclationProgress::CALCULATION_FAILED:
            statusBar()->showMessage(tr("Calculation failed"));
            break;
        case MessageCaluclationProgress::CALCULATION_ENDED:
            statusBar()->showMessage(tr("Calculation ended"));
            break;
        case MessageCaluclationProgress::CALCULATION_STARTED:
            statusBar()->showMessage(tr("Calculation started"));
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void MainWindow::changesInFileUpdated(bool) {
    updateWindowTitle();
}

void MainWindow::updateNoteName(int key) {
    QString text(QString::fromStdString(mCore->getPianoManager()->getPiano().getKeyboard().getNoteName(key)));
    // add super script to #
    text.replace("#", "<sup>#</sup>");
    //text.replace(QRegExp("([0-9])"), "<sub>\\1</sub>");
    mSignalAnalyzerGroup->setKey(text);
}

void MainWindow::updateFrequency(const Key *key) {
    if (!key) {
        mSignalAnalyzerGroup->setFrequency(QString("-"));
    } else {
        double frequency = key->getRecordedFrequency();
        if (mCurrentMode == MODE_TUNING) {
            frequency = key->getTunedFrequency();
        }
        mSignalAnalyzerGroup->setFrequency(QString("%1").arg(frequency, 0, 'f', 1));
    }
}

void MainWindow::updateWindowTitle() {
    QFileInfo fi(QString::fromStdWString(mCore->getProjectManager()->getCurrentFilePath()));
    QString asterix = mCore->getProjectManager()->hasChangesInFile() ? "*" : "";
    QString pianoName(QString::fromStdWString(mCore->getPianoManager()->getPiano().getName()));

    QString title = tr("Entropy piano tuner");
    if (pianoName.isEmpty() == false) {
        title = "[" + pianoName + "] - " + title;
    }
    if (fi.fileName().isEmpty() == false) {
        title = fi.fileName() + asterix + " - " + title;
    }
    setWindowTitle(title);
}

void MainWindow::updateVolumeBar() {
    mVolumeControlGroup->updateLevels(mCore->getAudioRecorder()->getStopLevel(), AudioRecorder::LEVEL_TRIGGER);
}

void MainWindow::onOpenSoundControl() {
    QList<QPair<QString, QStringList> > programNames;
#ifdef __linux__
    programNames.append(qMakePair(QString("pavucontrol"), QStringList()));
    programNames.append(qMakePair(QString("gnome-control-center"), QStringList() << "sound"));
#elif __APPLE__
    // not supported yet
    programNames.append(qMakePair(QString("open"), QStringList() << "/System/Library/PreferencePanes/Sound.prefPane/"));
#elif _WIN32
    // untested, see http://support.microsoft.com/kb/192806/de
    programNames.append(qMakePair(QString("control"), QStringList() << "mmsys.cpl" << "sounds"));
#endif

#ifdef QT_NO_PROCESS
    // no processes allowed on the platform (e.g. windows RT)
#else
    QProcess *process = new QProcess(this);
    for (auto &program : programNames) {
        process->start(program.first, program.second);
        if (process->waitForStarted()) {
            LogI("Sound control program '%s' started", program.first.toStdString().c_str());
            return;
        }
    }

    // no program could be started
    QMessageBox::warning(this, tr("Warning"), tr("Unable to find a supported sound control."));
#endif
}

bool MainWindow::isSoundControlSupported() {
#ifdef __ANDROID__
    return false;
#elif __APPLE__
#   if TARGET_IPHONE_SIMULATOR
    return false;
#   elif TARGET_OS_IPHONE
    return false;
#   endif
#endif

    return true;
}

void MainWindow::onResetNoiseLevel() {
    mCore->getAudioRecorder()->resetInputLevelControl();
}

void MainWindow::onToggleMicroMute(bool checked) {
    mCore->getAudioInput()->suspend(checked);
}

void MainWindow::onToggleSpeakerMute(bool checked) {
    mCore->getAudioPlayer()->suspend(checked);
}

void MainWindow::onFileNew() {
    mCore->getProjectManager()->onNewFile();
}

void MainWindow::onFileSave() {
    mCore->getProjectManager()->onSaveFile();
}

void MainWindow::onFileSaveAs() {
    mCore->getProjectManager()->onSaveFileAs();
}

void MainWindow::onFileOpen() {
    mCore->getProjectManager()->onOpenFile();
}

void MainWindow::onFileShare() {
    if (mCore->getProjectManager()->onShare() != ProjectManagerAdapter::R_ACCEPTED) {
        QMessageBox::information(this, tr("Canceled"), tr("You need to save the file before you can share it."));
    }
}

void MainWindow::onEditPianoDataSheet() {
    mCore->getProjectManager()->onEditFile();
}

void MainWindow::onResetRecording() {
    if (DoNotShowAgainMessageBox::show(DoNotShowAgainMessageBox::RESET_PITCHES, this) == QMessageBox::Yes) {
        mCore->getPianoManager()->resetPitches();
    }
}

void MainWindow::onOptions() {
    options::OptionsDialog options(this);
    options.exec();
}

void MainWindow::onOpenPlots() {
    PlotsDialog pd(mCore->getPianoManager()->getPiano(), this);
    pd.exec();
}

void MainWindow::onTutorial() {
    QDialog helpDialog(this, Qt::Window);

    // if in non maximized mode, the dialog has a size of half the main window
    QRect mwRect(this->geometry());
    helpDialog.setGeometry(mwRect.x() + mwRect.width() / 4, mwRect.y() + mwRect.height() / 4,
                           mwRect.width() / 2, mwRect.height() / 2);

    // create elements
    QVBoxLayout *mainLayout = new QVBoxLayout;
    helpDialog.setLayout(mainLayout);
    QTextBrowser *edit = new QTextBrowser;
    mainLayout->addWidget(edit);
    edit->setOpenExternalLinks(true);
    edit->setReadOnly(true);
    QString languageStr(QLocale().name());
    languageStr = languageStr.left(languageStr.indexOf("_"));
    QFile manual(":/tutorial/tutorial_" + languageStr + ".html");

    // function to read all text from the given file
    auto readAllFromFile = [](QFile &f) {
        QTextStream in(&f);
        return in.readAll();
    };

    // load manual logic (use english if language not found)
    QString text;
    if (!manual.open(QFile::ReadOnly | QFile::Text)) {
        LogW("Manual not found for language %s. Using english as fallback.",
                languageStr.toStdString().c_str());

        // use english as fallback
        QFile enManual(":/tutorial/tutorial_en.html");
        if (!enManual.open(QFile::ReadOnly | QFile::Text)) {
            LogE("Engish manual not found. Maybe the location of the manual changed?");
            text = "No manual found. Please use the online manual instead.";
        } else {
            text = readAllFromFile(enManual);
        }
    } else {
        text = readAllFromFile(manual);
    }

    // replace media content
    text.replace(QRegExp("SRC=\"([^\"]*)\""), "SRC=\":/tutorial/\\1\"");
    edit->setText(text);
    edit->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    mainLayout->addWidget(buttons);

    QObject::connect(buttons, SIGNAL(rejected()), &helpDialog, SLOT(reject()));

    QScroller::grabGesture(edit);

    // show maximized as default
    helpDialog.showMaximized();
    helpDialog.exec();
}

void MainWindow::onManual() {
    QDesktopServices::openUrl(QUrl(serverinfo::getManualFileAddress(SettingsForQt::getSingleton().getUserLanguageId()).c_str()));
}

void MainWindow::onAbout() {
    AboutDialog d(this, mIconPostfix);
    d.exec();
}


void MainWindow::onViewLog() {
    LogViewer logViewer(LogViewer::CURRENT_LOG, this);
    logViewer.exec();
}

void MainWindow::onModeIdle(bool checked) {
    if (checked) {
        setMode(MODE_IDLE);
    }
}

void MainWindow::onModeRecord(bool checked) {
    if (checked) {
        setMode(MODE_RECORDING);
    }
}

void MainWindow::onModeCalculate(bool checked) {
    if (checked) {
        setMode(MODE_CALCULATION);
    }
}

void MainWindow::onModeTune(bool checked) {
    if (checked) {
        setMode(MODE_TUNING);
    }
}

void MainWindow::onSelectNextMode() {
    OperationMode mode = static_cast<OperationMode>((mCurrentMode + 1) % MODE_COUNT);
    mModeToolButtons[mode]->setChecked(true);
}

void MainWindow::onSelectPreviousMode() {
    OperationMode mode = static_cast<OperationMode>((mCurrentMode - 1 + MODE_COUNT) % MODE_COUNT);
    mModeToolButtons[mode]->setChecked(true);
}

void MainWindow::onToggleFullscreen() {
    if (this->windowState() & Qt::WindowFullScreen) {
        showNormal();
    } else {
        showFullScreen();
    }
}


void MainWindow::onToggleTuningIndictator()
{
    if (mTuningIndicatorGroup)
         mTuningIndicatorGroup->getTunincIndicatorView()->toggleSpectralAndStroboscopeMode();
}


void MainWindow::onExport() {
    mCore->getProjectManager()->onExport();
}

void MainWindow::onVersionUpdate(VersionInformation information) {
    QMessageBox mb;
    mb.setText(tr("A new update is available!"));
    mb.setInformativeText(tr("The online app version is %1. Do you want to install this update?").arg(information.mAppVersion));
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    if (mb.exec() == QMessageBox::Yes) {
#ifdef Q_OS_MACX
        // download new dmg file
        if (!QDesktopServices::openUrl(QUrl::fromUserInput(serverinfo::getDownloadsFileAddress(serverinfo::MAC_X86_64_DMG).c_str()))) {
            LogW("Online installer file for mac could not be found.");
            QDesktopServices::openUrl(QUrl::fromUserInput(serverinfo::SERVER_ADDRESS.c_str()));
        } else {
            this->close();
        }
#else
#   ifdef QT_NO_PROCESS
        // no processes allowed on this device, usually mobile devices (e.g. WinRT/WinPhone)
#   else
        // run maintenace tool in updater mode
        if (QProcess::startDetached("maintenancetool", QStringList() << "--updater") == false) {
            LogW("Maintenace tool could not be started.");
            QMessageBox::warning(this, tr("Warning"), tr("The maintenance tool could not be started automatically. To update the program you have to start the maintenance tool manually."));
        } else {
            // close the program for the installer
            this->close();
        }
#   endif  // QT_NO_PROCESS

#endif  // Q_OS_MACX
    }
}

void MainWindow::onMidiInputDeviceCreated(const QMidiInput *input) {
    connect(input, &QMidiInput::notify, this, &MainWindow::onMidiMessageReceived);
}

void MainWindow::onMidiMessageReceived(const QMidiMessage &message) {
    mCore->getMidiInterface()->receiveMessage(message.byte0(), message.byte1(), message.byte2(), message.timestamp());
}
