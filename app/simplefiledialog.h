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

#ifndef SIMPLEFILEDIALOG_H
#define SIMPLEFILEDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QTreeWidget>
#include <QDir>
#include <QLineEdit>

#include "prerequisites.h"

class SimpleFileDialog : public QDialog
{
    Q_OBJECT
public:
    enum class Mode {
        Open,
        Save,
    };

public:
    SimpleFileDialog(Mode mode, QDir dir);

    static QString getOpenFile(QDir dir);
    static QString getSaveFile(QDir dir);

    QString getFileName() const {return mNameEdit->text() + ".ept";}

protected:
    void accept();

signals:
    void fileSelectionChanged(QString);
private slots:
    void onDeleteFile();
    void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    const Mode mMode;
    const QDir mDir;
    QTreeWidget *mFilesList;
    QLineEdit *mNameEdit;
};

#endif // SIMPLEFILEDIALOG_H
