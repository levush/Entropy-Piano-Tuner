/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
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
#include "../core/system/version.h"
#include <QDebug>
#include <QCloseEvent>
#include <QProcess>
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
#include "qtconfig.h"
#include "logforqt.h"
#include "editpianosheetdialog.h"
#include "logviewer.h"
#include "settingsforqt.h"
#include "options/optionsdialog.h"
#include "donotshowagainmessagebox.h"
#include "autoclosingmessagebox.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mIconPostfix((QApplication::primaryScreen()->devicePixelRatio() > 1.5) ? "@2x" : ""),
    mCore(nullptr),
    ui(new Ui::MainWindow),
    mSmallScreen(false)
{
    ui->setupUi(this);

    int iconSize = ui->modeToolBar->fontMetrics().height();
    QSize modeIconSize = QSize(iconSize, iconSize) * 2;
    ui->modeToolBar->setObjectName("modeToolBar");
    ui->modeToolBar->setIconSize(QSize(iconSize, iconSize) * 2);


    QScrollArea *modeScrollArea = new QScrollArea;
    ui->modeToolBar->addWidget(modeScrollArea);
    modeScrollArea->setWidgetResizable(true);
    modeScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    modeScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    modeScrollArea->setStyleSheet("\
                        QScrollArea { background: transparent; } \
                        QScrollArea > QWidget > QWidget { background: transparent; } \
                        ");
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
    mFileToolBar->addAction(iconFromTheme("document-properties"), tr("Edit piano data sheet"), this, SLOT(onEditPianoDataSheet()));
    mFileToolBar->addAction(iconFromTheme("preferences-system"), tr("Options"), this, SLOT(onOptions()));

    QToolBar *helpToolBar = new QToolBar(tr("Help"));
    helpToolBar->setObjectName("helpToolBar");
    addToolBar(Qt::TopToolBarArea, helpToolBar);
    helpToolBar->setMovable(false);
    helpToolBar->setFloatable(false);
    helpToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::LeftToolBarArea);
    helpToolBar->setIconSize(mFileToolBar->iconSize());

    QWidget *tbStretch = new QWidget;
    helpToolBar->addWidget(tbStretch);
    tbStretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    tbStretch->setVisible(true);

    helpToolBar->addAction(iconFromTheme("help-contents"), tr("Tutorial"), this, SLOT(onTutorial()));
    helpToolBar->addAction(iconFromTheme("accessories-text-editor"), tr("Log"), this, SLOT(onViewLog()));
    helpToolBar->addAction(iconFromTheme("help-about"), tr("About"), this, SLOT(onAbout()));


    // main menu icons
#if CONFIG_MENU_ICONS
    ui->action_New->setIcon(iconFromTheme("document-new"));
    ui->actionOpen->setIcon(iconFromTheme("document-open"));
    ui->actionSave->setIcon(iconFromTheme("document-save"));
    ui->actionSave_As->setIcon(iconFromTheme("document-save-as"));
    ui->actionExit->setIcon(iconFromTheme("application-exit"));
    ui->actionEdit_piano_data_sheet->setIcon(iconFromTheme("document-properties"));
    ui->actionReset_recoding->setIcon(iconFromTheme("edit-clear"));
    ui->actionOptions->setIcon(iconFromTheme("preferences-system"));
    ui->actionTutorial->setIcon(iconFromTheme("help-contents"));
    ui->actionView_log->setIcon(iconFromTheme("accessories-text-editor"));
    ui->actionAbout->setIcon(iconFromTheme("help-about"));
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
    new QShortcut(QKeySequence(Qt::Key_M), ui->muteButton, SLOT(toggle()));
    ui->action_New->setShortcut(QKeySequence::New);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSave_As->setShortcut(QKeySequence::SaveAs);
    ui->actionExit->setShortcut(QKeySequence::Quit);
    ui->actionOptions->setShortcut(QKeySequence::Preferences);
    ui->actionTutorial->setShortcut(QKeySequence::HelpContents);
    ui->actionEdit_piano_data_sheet->setShortcut(QKeySequence(Qt::Key_F9));

    ui->fourierSpectrumGraphics->setKeyboard(ui->keyboardGraphicsView);
    ui->tuningCurveGraphicsView->setKeyboard(ui->keyboardGraphicsView);

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


    qDebug() << "Display size: " << QGuiApplication::primaryScreen()->physicalSize();
    if (std::max(QGuiApplication::primaryScreen()->physicalSize().width(),
                 QGuiApplication::primaryScreen()->physicalSize().height()) < 150) {
        // smaller than 15 cm, use text abbrevs and hide some elements
        initForSmallScreen();
    }

    mCore->getProjectManager()->setCallback(this);

    // just for testing small screens on normal screen resolution
    // initForSmallScreen();

    ui->controlLayout->addWidget(mCalculationProgressGroup = new CalculationProgressGroup(mCore, mSmallScreen, this));
}

void MainWindow::start() {
    updateVolumeBar();

    // init the project manager (load startup file)
    mCore->getProjectManager()->init(mCore);

    updateWindowTitle();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (mCore->getProjectManager()->onQuit()) {
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
            WARNING("MessagePojectFile::Type Not implemented");
            break;
        }

        break;
    }
    case Message::MSG_MODE_CHANGED: {
        auto mmc(std::static_pointer_cast<MessageModeChanged>(m));

        // check the correct tool button
        mModeToolButtons[mmc->getMode()]->setChecked(true);

        // hide all groups first and enable if required
        ui->tuningGroupBox->setVisible(false);
        mCalculationProgressGroup->setVisible(false);
        ui->volumeGroupBox->setVisible(false);
        ui->signalAnalyzerGroupBox->setVisible(false);

        if (mmc->getMode() == MODE_IDLE) {
            ui->volumeGroupBox->setVisible(true);
            ui->signalAnalyzerGroupBox->setVisible(true);
        }

        if (mmc->getMode() == MODE_RECORDING) {
            ui->volumeGroupBox->setVisible(true);
            ui->signalAnalyzerGroupBox->setVisible(true);
        }
        else {
        }

        if (mmc->getMode() == MODE_CALCULATION) {
            // hide first for correct sizing
            mCalculationProgressGroup->setVisible(true);
            if (!mSmallScreen) {
                ui->fourierSpectrumGraphics->setVisible(false);
            }
        } else {
            // hide first for correct sizing
            if (!mSmallScreen) {
                ui->fourierSpectrumGraphics->setVisible(true);
            }
        }

        if (mmc->getMode() == MODE_TUNING) {
            ui->tuningGroupBox->setVisible(true);
            ui->volumeGroupBox->setVisible(true);
            ui->signalAnalyzerGroupBox->setVisible(true);

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

        updateFrequency(ui->keyboardGraphicsView->getSelectedKey());
        break;
    }
    case Message::MSG_RECORDING_STARTED:
        statusBar()->showMessage(tr("Recording keystroke"));
        if (mSmallScreen) {
            ui->tuningCurveGraphicsView->setVisible(false);
            ui->fourierSpectrumGraphics->setVisible(true);
        }
        ui->frequencyValueLabel->setText("-");
        break;
    case Message::MSG_SIGNAL_ANALYSIS_STARTED:
        statusBar()->showMessage(tr("Signal analysis started"));
        break;
    case Message::MSG_SIGNAL_ANALYSIS_ENDED:
        statusBar()->showMessage(tr("Signal analysis ended"));
        if (mSmallScreen) {
            ui->fourierSpectrumGraphics->setVisible(false);
            ui->tuningCurveGraphicsView->setVisible(true);
        }
        break;
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

void MainWindow::initForSmallScreen() {
    mSmallScreen = true;

    // hide some elements
    ui->fourierSpectrumGraphics->setVisible(false);          // only visible during recording

    ui->keyboardGraphicsView->setMode(KeyboardGraphicsView::MODE_CLICK_RAISES_FULLSCREEN);

    // hide status bar
    statusBar()->setHidden(true);
}

void MainWindow::updateNoteName(int key) {
    QString text(QString::fromStdString(mCore->getPianoManager()->getPiano().getKeyboard().getNoteName(key)));
    // add super script to #
    text.replace("#", "<sup>#</sup>");
    //text.replace(QRegExp("([0-9])"), "<sub>\\1</sub>");
    ui->recordingCurrentKeyLabel->setText(text);
}

void MainWindow::updateFrequency(const Key *key) {
    if (!key) {
        ui->frequencyValueLabel->setText(QString("-"));
    } else {
        double frequency = key->getRecordedFrequency();
        if (mCurrentMode == MODE_TUNING) {
            frequency = key->getTunedFrequency();
        }
        ui->frequencyValueLabel->setText(QString("%1").arg(frequency, 0, 'f', 1));
    }
}

void MainWindow::updateWindowTitle() {
    setWindowTitle(tr("Entropy piano tuner") + " - " + QString::fromStdString(mCore->getPianoManager()->getPiano().getName()) + (mCore->getProjectManager()->hasChangesInFile() ? "*" : ""));
}

void MainWindow::updateVolumeBar() {
    int max = 10000;
    int pos0 = mCore->getAudioRecorder()->getStopLevel() * max;
    int pos1 = AudioRecorderAdapter::LEVEL_TRIGGER * max;

    ui->volumeControlLabelsLayout->setStretch(0, pos0);
    ui->volumeControlLabelsLayout->setStretch(1, pos1 - pos0);
    ui->volumeControlLabelsLayout->setStretch(2, max - pos1);

    ui->volumeControlLinesLayout->setStretch(0, pos0);
    ui->volumeControlLinesLayout->setStretch(1, 0);
    ui->volumeControlLinesLayout->setStretch(2, pos1 - pos0);
    ui->volumeControlLinesLayout->setStretch(3, 0);
    ui->volumeControlLinesLayout->setStretch(4, max - pos1);
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
            INFORMATION("Sound control program '%s' started", program.first.toStdString().c_str());
            return;
        }
    }

    // no program could be started
    QMessageBox::warning(this, tr("Warning"), tr("Unable to find a supported sound conrol."));
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
    mCore->getAudioRecorder()->resetNoiseLevel();
}

void MainWindow::onToggleMute(bool checked) {
    mCore->getAudioRecorder()->setMuted(checked);
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
    if (DoNotShowAgainMessageBox::show(DoNotShowAgainMessageBox::RESET_RECORDING, this) == QMessageBox::Accepted) {
        mCore->getPianoManager()->clear();
    }
}

void MainWindow::onOptions() {
    options::OptionsDialog options(this);
    options.exec();
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
    QResource manual(":/tutorial/tutorial_" + languageStr + ".html");
    QString text;
    if (manual.isValid() == false) {
        WARNING("Manual not found for language %s. Using english as fallback.",
                languageStr.toStdString().c_str());

        // use english as fallback
        QResource enManual(":/tutorial/tutorial_en.html");
        if (enManual.isValid() == false) {
            ERROR("Engish manual not found. Maybe the location of the manual changed?");
            text = "No manual found. Please use the online manual instead.";
        } else {
            text = QByteArray(reinterpret_cast<const char*>(enManual.data()), enManual.size());
        }
    } else {
        text = QByteArray(reinterpret_cast<const char*>(manual.data()), manual.size());
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
    QDesktopServices::openUrl(QUrl(QString("http://www.entropy-tuner.org/www/manual/manual_%1.pdf").arg(QString::fromStdString(SettingsForQt::getSingleton().getUserLanguageId()))));
}

void MainWindow::onAbout() {
    QDialog d(this, Qt::Window);
    QRect r(this->geometry());
    d.setGeometry(r.left() + r.width() / 4, r.top() + r.height() / 4, r.width() / 2, r.height() / 2);
    d.setWindowTitle(tr("About"));
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    d.setLayout(mainLayout);

    mainLayout->addWidget(new QLabel(QString("<h1>%1 %2</h1>").arg(tr("Entropy Piano Tuner"), EPT_VERSION_STRING)));

    QString iconPath = ":/media/images/icon_256x256" + mIconPostfix + ".png";
    QTextBrowser *text = new QTextBrowser;
    text->setStyleSheet("background-color: transparent;");
    text->setFrameShape(QFrame::NoFrame);
    text->setOpenLinks(false);
    QObject::connect(text, SIGNAL(anchorClicked(QUrl)), this, SLOT(onOpenAboutUrl(QUrl)));
    mainLayout->addWidget(text);

    const QString buildText = tr("Built on %1").arg(QDateTime::fromString(__TIMESTAMP__).toString(Qt::DefaultLocaleLongDate));
    const QString buildByText = tr("by %1 and %2").arg("Prof. Dr. Haye Hinrichsen", "Christoph Wick M.Sc.");

    QString dependenciesText = tr("Based on");
    dependenciesText.append(" <a href=\"Qt\">Qt</a>, <a href=\"http://fftw.org\">fftw3</a>");
    dependenciesText.append(", <a href=\"http://www.grinninglizard.com/tinyxml2\">tinyxml2</a>");
    dependenciesText.append(", <a href=\"http://www.music.mcgill.ca/~gary/rtmidi\">RtMidi</a>");

    const QString copyrightText = tr("Copyright 2015 Dept. of Theor. Phys. III, University of Würzburg. All rights reserved.");
    const QString licenseText = tr("This software is licensed unter the terms of the %1. The source code can be accessed at %2.").
            arg("<a href=\"http://www.gnu.org/licenses/gpl-3.0-standalone.html\">GPLv3</a>",
                "<a href=\"https://gitlab.com/entropytuner/Entropy-Piano-Tuner\">GitLab</a>");

    const QString warrantyText = tr("The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.");

    const QString acknowledgementsText = tr("We thank all those who have contributed to the project:") +
           " Prof. Dr. S. R. Dahmen, A. Frick, A. Heilrath, M. Jiminez, Prof. Dr. W. Kinzel, M. Kohl, L. Kusmierz, Prof. Dr. A. C. Lehmann, B. Olbrich.";

    auto makeParagraphTags = [](const QString &t) {return "<p>" + t + "</p>";};
    QString completeText;
    completeText.append("<html><img src=\"" + iconPath + "\" style=\"float: left;\"/>");

    completeText.append(makeParagraphTags(buildText));
    completeText.append(makeParagraphTags(buildByText));
    completeText.append(makeParagraphTags(dependenciesText));
    completeText.append(makeParagraphTags(copyrightText));
    completeText.append(makeParagraphTags(licenseText));
    completeText.append(makeParagraphTags(warrantyText));
    completeText.append(makeParagraphTags(acknowledgementsText));

    completeText.append("</html>");

    text->setHtml(completeText);

    QHBoxLayout *okButtonLayout = new QHBoxLayout;
    okButtonLayout->setMargin(0);
    mainLayout->addLayout(okButtonLayout);
    okButtonLayout->addStretch();

    QPushButton *okButton = new QPushButton(tr("Ok"));
    okButtonLayout->addWidget(okButton);

    QObject::connect(okButton, SIGNAL(clicked()), &d, SLOT(accept()));

    QScroller::grabGesture(text);
    text->setReadOnly(true);
    text->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);

    SHOW_DIALOG(&d);

    d.exec();
}

void MainWindow::onOpenAboutUrl(QUrl url) {
    if (url.toString() == "Qt") {
        QMessageBox::aboutQt(this);
    } else {
        QDesktopServices::openUrl(url);
    }
}

void MainWindow::onViewLog() {
    LogViewer logViewer(this);
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
