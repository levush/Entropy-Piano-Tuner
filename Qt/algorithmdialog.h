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

#ifndef ALGORITHMDIALOG_H
#define ALGORITHMDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QList>
#include <QPair>
#include <QScrollArea>
#include <QComboBox>
#include <memory>

#include "core/calculation/algorithminformation.h"

class AlgorithmParameter;
class AlgorithmFactoryDescription;

class AlgorithmDialog : public QDialog
{
    Q_OBJECT
public:
    AlgorithmDialog(QString currentAlgorithm, QWidget *parent);

    const QString getAlgorithm() const {return mCurrenctAlgorithm;}
private:
    void acceptCurrent();

private slots:
    void algorithmSelectionChanged(int index);
    void accept() override;

private:
    using AlgorithmIdList = QList<QPair<QString, QString>>;
    static AlgorithmIdList mAlgorithmNames;

    using AlgorithmWidgetConnectionList = QList<QPair<std::string, QWidget*>>;
    AlgorithmWidgetConnectionList mAlgorithmWidgetConnectionList;

    AlgorithmFactoryDescription *mCurrenctFactoryDescription = nullptr;
    std::unique_ptr<const AlgorithmInformation> mCurrentAlgorithmInformation;
    QComboBox *mAlgorithmSelection = nullptr;
    QScrollArea *mAlgorithmDescriptionScrollArea = nullptr;

    QString mCurrenctAlgorithm;
};

#endif // ALGORITHMDIALOG_H
