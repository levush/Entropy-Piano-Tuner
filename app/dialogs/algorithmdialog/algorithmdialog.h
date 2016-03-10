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

#ifndef ALGORITHMDIALOG_H
#define ALGORITHMDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QList>
#include <QPair>
#include <QScrollArea>
#include <QComboBox>
#include <QPushButton>
#include <memory>

#include "prerequisites.h"
#include "core/calculation/algorithminformation.h"
#include "core/piano/piano.h"


class AlgorithmParameter;
class AlgorithmFactoryDescription;

class AlgorithmDialog : public QDialog
{
    Q_OBJECT
public:
    AlgorithmDialog(std::shared_ptr<const AlgorithmInformation> currentAlgorithm, Piano &piano, QWidget *parent);

    std::shared_ptr<const AlgorithmInformation> getAlgorithmInformation() const {return mCurrentAlgorithmInformation;}
private:
    void acceptCurrent();
    virtual bool eventFilter(QObject *o, QEvent *e) override final;

private slots:
    void algorithmSelectionChanged(int index);
    void defaultButtonClicked();
    void accept() override;

private:
    using AlgorithmIdList = QList<QPair<QString, QString>>;
    static AlgorithmIdList mAlgorithmNames;

    using AlgorithmWidgetConnectionList = QList<QPair<std::string, QWidget*>>;
    AlgorithmWidgetConnectionList mAlgorithmWidgetConnectionList;

    std::shared_ptr<const AlgorithmInformation> mCurrentAlgorithmInformation;
    Piano &mPiano;
    SingleAlgorithmParametersPtr mCurrentAlgorithmParameters;
    QComboBox *mAlgorithmSelection = nullptr;
    QScrollArea *mAlgorithmDescriptionScrollArea = nullptr;

    class DefaultButton : public QPushButton {
    public:
        DefaultButton(QString label, const std::string &id, QWidget *dataWidget) :
            QPushButton(label),
            mId(id),
            mDataWidget(dataWidget) {
        }

        const std::string &getID() const {return mId;}
        QWidget *getDataWidget() const {return mDataWidget;}
    private:
        const std::string &mId;
        QWidget *mDataWidget;
    };
};

#endif // ALGORITHMDIALOG_H
