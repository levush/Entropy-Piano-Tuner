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

#include "simplefiledialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>
#include <QGuiApplication>
#include <QPushButton>
#include <QHeaderView>
#include <QToolButton>
#include <QMessageBox>
#include <QScroller>

#include "qtconfig.h"
#include "core/system/eptexception.h"
#include "core/system/log.h"

SimpleFileDialog::SimpleFileDialog(Mode mode, QDir dir) :
    QDialog(0, Qt::Window),
    mMode(mode),
    mDir(dir)
{
    setModal(true);

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    QDialogButtonBox *buttons = new QDialogButtonBox;
    QString title;
    if (mode == Mode::Open) {
        buttons->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Open);
        title = buttons->button(QDialogButtonBox::Open)->text();
    } else {
        buttons->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Save);
        title = buttons->button(QDialogButtonBox::Save)->text();
    }
    title.remove('&');

    layout->addWidget(new QLabel(QString("<h1>%1</h1>").arg(title)));

    mFilesList = new QTreeWidget;
    layout->addWidget(mFilesList);
    mFilesList->setColumnCount(2);
    mFilesList->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    mFilesList->header()->setSectionResizeMode(1, QHeaderView::Fixed);
    mFilesList->header()->setVisible(false);

    QStringList files = dir.entryList(QDir::Files | QDir::Writable);
    int itemSize = mFilesList->fontMetrics().height();
    for (QString file : files) {
        if (file.endsWith(".ept") == false) {
            continue;
        }
        file = file.left(file.size() - 4);
        QTreeWidgetItem *item = new QTreeWidgetItem(mFilesList, QStringList() << file);
        item->setSizeHint(0, QSize(0, itemSize * 2));
        QToolButton *tb = new QToolButton;
        tb->setAutoRaise(true);
        tb->setIcon(QIcon(":/media/icons/edit-delete.png"));
        tb->setIconSize(QSize(itemSize, itemSize) * 1.7);
        QObject::connect(tb, SIGNAL(clicked(bool)), this, SLOT(onDeleteFile()));
        mFilesList->setItemWidget(item, 1, tb);
    }
    mFilesList->setIconSize(QSize(64, 64));
    mFilesList->setCurrentItem(mFilesList->topLevelItem(0));

    mFilesList->header()->setStretchLastSection(false);
    mFilesList->header()->resizeSection(1, itemSize * 2);
    mFilesList->header()->resizeSection(0, QHeaderView::Stretch);

    QHBoxLayout *nameLayout = new QHBoxLayout(this);
    nameLayout->addWidget(new QLabel(tr("Name:")));

    QLineEdit *nameEdit = new QLineEdit;
    mNameEdit = nameEdit;
    nameLayout->addWidget(nameEdit);

    if (mode == Mode::Save) {
        layout->addLayout(nameLayout);
    }

    layout->addWidget(buttons);


    QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(mFilesList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(accept()));
    QObject::connect(mFilesList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    QObject::connect(this, SIGNAL(fileSelectionChanged(QString)), nameEdit, SLOT(setText(QString)));

    SHOW_DIALOG(this);

    if (mode == Mode::Save) {
        mNameEdit->setText(QString());
    } else {
        if (mFilesList->currentItem()) {
            mNameEdit->setText(mFilesList->currentItem()->text(0));
        }
    }

    QScroller::grabGesture(mFilesList);
}

QString SimpleFileDialog::getOpenFile(QDir dir) {
    SimpleFileDialog dialog(Mode::Open, dir);
    if (dialog.exec() == QDialog::Rejected) {
        return QString();
    }

    return dir.absoluteFilePath(dialog.getFileName());
}

QString SimpleFileDialog::getSaveFile(QDir dir) {
    SimpleFileDialog dialog(Mode::Save, dir);
    if (dialog.exec() == QDialog::Rejected) {
        return QString();
    }

    return dir.absoluteFilePath(dialog.getFileName());
}

void SimpleFileDialog::accept() {
    const QFileInfo fi(mDir.absoluteFilePath(getFileName()));
    if (mMode == Mode::Save) {
        // check if the file already exists
        if (fi.exists()) {
            if (QMessageBox::warning(this,
                                     tr("File existing"),
                                     tr("A file with the given name already exists at %1. Do you want to overwrite it?").arg(fi.absoluteFilePath()),
                                     QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
                return;
            }
        }
    }

    if (fi.baseName().isEmpty()) {
        QMessageBox::information(this, tr("Invalid filename"), tr("Please provide a valid filename."));
    } else {
        QDialog::accept();
    }
}

void SimpleFileDialog::onDeleteFile() {
    EptAssert(sender(), "This is a slot and has to be sent by a QPushButton.");

    QTreeWidgetItem *fileItem = nullptr;
    for (int i = 0; i < mFilesList->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = mFilesList->topLevelItem(i);
        if (sender() == mFilesList->itemWidget(item, 1)) {
            fileItem = item;
            break;
        }
    }

    EptAssert(fileItem, "Item not found that contains the sender in column 1.");

    // ask for confirmation
    if (QMessageBox::question(this, tr("Remove file"),
                              tr("Are you sure that you really want to delete the file \"%1\"?").arg(fileItem->text(0)),
                              QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel) {
        // cancel
        return;
    }

    QFile file(mDir.absoluteFilePath(fileItem->text(0) + ".ept"));

    EptAssert(file.exists(), "File has to exists.");

    if (!file.remove()) {
        LogW("File '%s' could not be removed.", file.fileName().toStdString().c_str());
        return;
    }

    mFilesList->removeItemWidget(fileItem, 1);
    delete fileItem;
}

void SimpleFileDialog::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);

    if (!current) {
        emit fileSelectionChanged(QString());
    } else {
        emit fileSelectionChanged(current->text(0));
    }
}
