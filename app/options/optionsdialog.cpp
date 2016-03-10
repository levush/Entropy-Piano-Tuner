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

#include "optionsdialog.h"
#include <QMessageBox>
#include <QSpacerItem>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolBar>
#include <assert.h>
#include <QDebug>

#include "core/adapters/projectmanageradapter.h"
#include "core/messages/messagehandler.h"
#include "core/system/log.h"
#include "core/config.h"
#include "core/core.h"

#include "audio/optionspageaudio.h"
#include "environment/optionspageenvironment.h"
#include "implementations/settingsforqt.h"
#include "mainwindow/mainwindow.h"
#include "displaysize.h"
#include "qtconfig.h"

namespace options {

OptionsDialog::OptionsDialog(MainWindow *mainWindow)
    : QDialog(mainWindow, Qt::Window),
      mMainWindow(mainWindow),
      mCore(mainWindow->getCore()),
      mCentralWidget(0),
      mChangesMade(false) {
    setWindowTitle(tr("Options"));
    setModal(true);

    mMainLayout = new QGridLayout(this);

    mMainLayout->addWidget(mTitle = new QLabel(), 0, 1);
    QFont titleFont(mTitle->font());
    titleFont.setPointSizeF(titleFont.pointSizeF() * 1.5);
    titleFont.setBold(true);
    mTitle->setFont(titleFont);

    QButtonGroup *group = new QButtonGroup(this);
    mPageButtons = group;

    QToolBar *selectionToolBar = new QToolBar;
    //selectionToolBar->setAllowedAreas(Qt::LeftToolBarArea);
    //selectionToolBar->setFloatable(true);
    selectionToolBar->setOrientation(Qt::Vertical);
    int iconSize = selectionToolBar->fontMetrics().height() * 2;
    selectionToolBar->setIconSize(QSize(iconSize, iconSize));
    mMainLayout->addWidget(selectionToolBar, 1, 0);

    addPageButton(QIcon(":/media/icons/user-desktop.png"), tr("Environment"), group, selectionToolBar, PAGE_ENVIRONMENT);
    addPageButton(QIcon::fromTheme("audio-card", QIcon(":/media/icons/audio-card.png")), tr("Audio"), group, selectionToolBar, PAGE_AUDIO);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel, this);
    mMainLayout->addWidget(buttonBox, 2, 1);
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(onApply()));

    setLayout(mMainLayout);

    int startupId = std::max<int>(0, SettingsForQt::getSingleton().getLastVisitedOptionsPage());
    group->button(startupId)->setChecked(true);
    onCurrentSelectionChanged(startupId);
    QObject::connect(group, SIGNAL(buttonClicked(int)), this, SLOT(onCurrentSelectionChanged(int)));

    resize(mMainWindow->centralWidget()->size());
    SHOW_DIALOG(this);
}

OptionsDialog::~OptionsDialog()
{
}

void OptionsDialog::accept() {
    onApply();
    QDialog::accept();
}

void OptionsDialog::reject() {
    if (!checkForChanges(true)) {return;}

    QDialog::reject();
}

void OptionsDialog::onCurrentSelectionChanged(int index) {
    checkForChanges(false);

    clearPages();


    mChangesMade = false;

    switch (index) {
    case PAGE_ENVIRONMENT:
        mCentralWidget = new PageEnvironment(this);
        break;
    case PAGE_AUDIO:
        mCentralWidget = new PageAudio(this);
        break;
    default:
        LogW("Message page %d not implemented.", index);
        return;
    }

    assert(mCentralWidget);

    mMainLayout->addWidget(mCentralWidget->getWidget(), 1, 1);

    mCentralWidget->getWidget()->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    mTitle->setText(mPageButtons->checkedButton()->text());

    SettingsForQt::getSingleton().setLastVisitedOptionsPage(index);
}

void OptionsDialog::onApply() {
    assert(mCentralWidget);
    if (mChangesMade) {
        mCentralWidget->apply();
        mChangesMade = false;
        MessageHandler::send(Message::MSG_OPTIONS_CHANGED);  // send a message

    }
}

void OptionsDialog::onChangesMade() {
    mChangesMade = true;
}

bool OptionsDialog::checkForChanges(bool allowCancel) {
    if (mChangesMade) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(this->windowTitle());
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText("There are unsaved changes.");
        msgBox.setInformativeText("Do you want to save your changes?");
        if (allowCancel) {
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        } else {
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        }

        switch (msgBox.exec()) {
        case QMessageBox::Yes:
            onApply();
            break;
        case QMessageBox::Cancel:
            return false;
        default:
            break;
        }
        mChangesMade = false;
    }

    return true;
}

void OptionsDialog::clearPages() {
    if (mCentralWidget) {
        mMainLayout->removeWidget(mCentralWidget->getWidget());
        delete mCentralWidget;
        mCentralWidget = nullptr;
    }
}

void OptionsDialog::addPageButton(QIcon icon, QString text, QButtonGroup *group, QToolBar *tb, OptionPages page) {
    QToolButton *button = new QToolButton(tb);
    button->setIcon(icon);
    button->setText(text);
    button->setCheckable(true);
    button->setChecked(false);
    button->setStyleSheet("text-align: left");
    button->setToolButtonStyle(DisplaySizeDefines::getSingleton()->optionsDialogToolButtonStyle());
    button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    button->setIconSize(tb->iconSize());
    group->addButton(button, page);
    tb->addWidget(button);
}

}  // options

