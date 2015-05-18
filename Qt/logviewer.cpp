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

#include "logviewer.h"
#include <QTextStream>
#include <QFile>
#include "../core/config.h"
#include "logforqt.h"
#include "filemanagerforqt.h"
#include "ui_logviewer.h"
#include "qtconfig.h"

LogViewer::LogViewer(QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::LogViewer)
{
    ui->setupUi(this);

    QFile f(QString::fromStdString(FileManager::getSingleton().getLogFilePath(LogForQt::LOG_NAME)));
    if (!f.open(QFile::ReadOnly | QFile::Text)) return;
    QTextStream in(&f);
    ui->textBrowser->setText(in.readAll());

    ui->textBrowser->setReadOnly(true);
    ui->textBrowser->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);

    SHOW_DIALOG(this);
}

LogViewer::~LogViewer()
{
    delete ui;
}
