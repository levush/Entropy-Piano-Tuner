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

#include "editpianosheetdialog.h"
#include <assert.h>
#include <QDate>
#include <QDateTime>
#include <QKeyEvent>
#include <QDebug>
#include "ui_editpianosheetdialog.h"
#include "../core/config.h"
#include "../core/piano/piano.h"
#include "qtconfig.h"

EditPianoSheetDialog::EditPianoSheetDialog(const Piano &piano, QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::EditPianoSheetDialog)
{
    ui->setupUi(this);

    ui->nameLineEdit->setText(QString::fromStdString(piano.getName()));
    ui->pianoType->setCurrentIndex(piano.getPianoType());
    ui->serialNumberLineEdit->setText(QString::fromStdString(piano.getSerialNumber()));
    // we only need the year
    ui->yearEdit->setDate(QDate(QString::fromStdString(piano.getManufactionYear()).toInt(), 1, 1));
    ui->productionLocationLineEdit->setText(QString::fromStdString(piano.getManufactionLocation()));

    ui->tuningLocationLineEdit->setText(QString::fromStdString(piano.getTuningLocation()));
    QDateTime time = QDateTime::fromString(QString::fromStdString(piano.getTuningTime()), "yyyy-MM-dd HH:mm:ss").toLocalTime();
    time.setTimeSpec(Qt::UTC);
    ui->timeOfTuningDateTimeEdit->setDateTime(time.toLocalTime());

    ui->concertPitchSpinBox->setValue(piano.getConcertPitch());
    ui->numberOfKeysSpinBox->setValue(piano.getKeyboard().getNumberOfKeys());
    ui->keyNumberOfASpinBox->setValue(piano.getKeyboard().getKeyNumberOfA4());

    SHOW_DIALOG(this);
}

EditPianoSheetDialog::~EditPianoSheetDialog()
{
    delete ui;
}

void EditPianoSheetDialog::applyData(Piano *piano) const {
    assert(piano);

    piano->setName(ui->nameLineEdit->text().toStdString());
    piano->setType(static_cast<piano::PianoType>(ui->pianoType->currentIndex()));
    piano->setSerialNumber(ui->serialNumberLineEdit->text().toStdString());
    // we only need the year
    piano->setManufactureYear(QString("%1").arg(ui->yearEdit->date().year()).toStdString());
    piano->setManufactureLocation(ui->productionLocationLineEdit->text().toStdString());

    piano->setTuningLocation(ui->tuningLocationLineEdit->text().toStdString());
    piano->setTuningTime(ui->timeOfTuningDateTimeEdit->dateTime().toUTC().toString("yyyy-MM-dd HH:mm:ss").toStdString());
    piano->setConcertPitch(ui->concertPitchSpinBox->value());
    piano->getKeyboard().setNumberOfKeys(ui->numberOfKeysSpinBox->value(),
                                         ui->keyNumberOfASpinBox->value());
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
    ui->keyNumberOfASpinBox->setValue(Piano::DEFAULT_KEY_NUMBER_OF_A);
}
