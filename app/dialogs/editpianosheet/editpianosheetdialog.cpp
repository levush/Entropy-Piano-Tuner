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

#include "editpianosheetdialog.h"
#include <assert.h>
#include <QDate>
#include <QDateTime>
#include <QKeyEvent>
#include <QDebug>
#include <QScrollBar>
#include "ui_editpianosheetdialog.h"
#include "../core/config.h"
#include "../core/piano/piano.h"
#include "qtconfig.h"
#include "displaysize.h"
#include "QScroller"

EditPianoSheetDialog::EditPianoSheetDialog(const Piano &piano, QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::EditPianoSheetDialog)
{
    ui->setupUi(this);
    setModal(true);

    // if (DisplaySizeDefines::getSingleton()->isLEq(DS_XSMALL)) {
    //    // on small devices we need a bit more space for translated languages (e.g. German), reduce margins in general for xsmall devices
    //    layout()->setMargin(3);
    //    ui->scrollAreaWidgetContents->layout()->setMargin(1);
    // }

    ui->scrollArea->setFrameShape(QFrame::NoFrame);

    ui->nameLineEdit->setText(QString::fromStdWString(piano.getName()));
    ui->pianoType->setCurrentIndex(piano.getPianoType());
    ui->serialNumberLineEdit->setText(QString::fromStdWString(piano.getSerialNumber()));
    // we only need the year
    ui->yearEdit->setDate(QDate(QString::fromStdWString(piano.getManufactionYear()).toInt(), 1, 1));
    ui->productionLocationLineEdit->setText(QString::fromStdWString(piano.getManufactionLocation()));

    ui->tuningLocationLineEdit->setText(QString::fromStdWString(piano.getTuningLocation()));
    QDateTime time = QDateTime::fromString(QString::fromStdWString(piano.getTuningTime()), "yyyy-MM-dd HH:mm:ss").toLocalTime();
    time.setTimeSpec(Qt::UTC);
    ui->timeOfTuningDateTimeEdit->setDateTime(time.toLocalTime());

    ui->concertPitchSpinBox->setValue(piano.getConcertPitch());
    ui->numberOfKeysSpinBox->setValue(piano.getKeyboard().getNumberOfKeys());
    ui->keyNumberOfASpinBox->setValue(piano.getKeyboard().getKeyNumberOfA4() + 1);  // counting starts at 1 (not at 0)
    ui->keysOnBassBridgeSpinBox->setValue(piano.getKeyboard().getNumberOfBassKeys());

    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScroller::grabGesture(ui->scrollArea->viewport(), QScroller::LeftMouseButtonGesture);

    if (DisplaySizeDefines::getSingleton()->showMultiLineEditPianoDataSheet()) {
        ui->pianoManufacturerInformationLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
        ui->pianoOperatingSiteInformationLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
    } else {
        ui->pianoManufacturerInformationLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
        ui->pianoOperatingSiteInformationLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    }

    ui->scrollAreaWidgetContents->installEventFilter(this);

    SHOW_DIALOG(this);
}

EditPianoSheetDialog::~EditPianoSheetDialog()
{
    delete ui;
}

void EditPianoSheetDialog::applyData(Piano *piano) const {
    assert(piano);

    piano->setName(ui->nameLineEdit->text().toStdWString());
    piano->setType(static_cast<piano::PianoType>(ui->pianoType->currentIndex()));
    piano->setSerialNumber(ui->serialNumberLineEdit->text().toStdWString());
    // we only need the year
    piano->setManufactureYear(QString("%1").arg(ui->yearEdit->date().year()).toStdWString());
    piano->setManufactureLocation(ui->productionLocationLineEdit->text().toStdWString());

    piano->setTuningLocation(ui->tuningLocationLineEdit->text().toStdWString());
    piano->setTuningTime(ui->timeOfTuningDateTimeEdit->dateTime().toUTC().toString("yyyy-MM-dd HH:mm:ss").toStdWString());
    piano->setConcertPitch(ui->concertPitchSpinBox->value());
    piano->getKeyboard().setNumberOfBassKeys(ui->keysOnBassBridgeSpinBox->value());

    piano->getKeyboard().changeKeyboardConfiguration(ui->numberOfKeysSpinBox->value(),
                                         ui->keyNumberOfASpinBox->value() - 1);  // counting start from 1 to 0

}

void EditPianoSheetDialog::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (!focusWidget() || focusWidget() == this || focusWidget() == ui->scrollArea) {
            QDialog::keyPressEvent(event);
        }
        break;
    default:
        QDialog::keyPressEvent(event);
        break;
    }
}

bool EditPianoSheetDialog::eventFilter(QObject *o, QEvent *e) {
    if (o == ui->scrollAreaWidgetContents && e->type() == QEvent::Resize) {
        setMinimumWidth(ui->scrollAreaWidgetContents->minimumSizeHint().width()
                        + ui->scrollArea->verticalScrollBar()->width()
                        + ui->scrollAreaWidgetContents->layout()->margin() * 2);
    }

    return false;
}

void EditPianoSheetDialog::onSetTuningTimeToNow() {
    ui->timeOfTuningDateTimeEdit->setDateTime(QDateTime::currentDateTime());
}

void EditPianoSheetDialog::onNumberOfKeysChanged(int k) {
    ui->keyNumberOfASpinBox->setMaximum(k);
}

void EditPianoSheetDialog::onDefaultConcertPitch() {
    ui->concertPitchSpinBox->setValue(Piano::DEFAULT_CONCERT_PITCH);
}

void EditPianoSheetDialog::onDefaultNumberOfKeys() {
    ui->numberOfKeysSpinBox->setValue(Piano::DEFAULT_NUMBER_OF_KEYS);
}

void EditPianoSheetDialog::onDefaultKeyNumberOfA() {
    ui->keyNumberOfASpinBox->setValue(Piano::DEFAULT_KEY_NUMBER_OF_A + 1);
}

void EditPianoSheetDialog::onDefaultKeysOnBassBridge() {
    ui->keysOnBassBridgeSpinBox->setValue(Piano::DEFAULT_KEYS_ON_BASS_BRIDGE);
}
