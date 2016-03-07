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

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QAudioDeviceInfo>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QButtonGroup>

#include "prerequisites.h"

class Core;
class AudioBase;
class AudioRecorderAdapter;
class AudioPlayerAdapter;
class MainWindow;

namespace options
{

class CentralWidgetInterface;

class OptionsDialog : public QDialog
{
    Q_OBJECT
public:
    enum OptionPages
    {
        PAGE_ENVIRONMENT = 0,
        PAGE_AUDIO,
    };

    OptionsDialog(MainWindow *mainWindow);
    virtual ~OptionsDialog();


    Core *getCore() const {return mCore;}
    MainWindow *getMainWindow() const {return mMainWindow;}

protected:
    void accept() override final;
    void reject() override final;

protected slots:
    void onCurrentSelectionChanged(int);
    void onApply();
    void onChangesMade();

private:
    bool checkForChanges(bool allowCancel);
    void clearPages();
    void addPageButton (QIcon icon, QString text, QButtonGroup *group, QToolBar *tb, OptionPages page);

private:
    MainWindow *mMainWindow;
    Core *mCore;
    QGridLayout *mMainLayout;
    CentralWidgetInterface *mCentralWidget;
    QLabel *mTitle;
    QButtonGroup *mPageButtons;
    bool mChangesMade;
};

}  // namespace options

#endif // OPTIONSDIALOG_H
