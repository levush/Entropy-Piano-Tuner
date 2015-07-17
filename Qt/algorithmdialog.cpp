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

#include "algorithmdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QScrollArea>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QScroller>
#include "doubleslider.h"

#include "qtconfig.h"
#include "core/config.h"
#include "core/calculation/algorithmfactorydescription.h"
#include "core/calculation/calculationmanager.h"
#include "core/calculation/algorithminformation.h"

AlgorithmDialog::AlgorithmIdList AlgorithmDialog::mAlgorithmNames;

AlgorithmDialog::AlgorithmDialog(std::shared_ptr<const AlgorithmInformation> currentAlgorithm, QWidget *parent) :
    QDialog(parent)
{
    EptAssert(currentAlgorithm, "Current algorithm has to exist.");

    QRect pr(parent->contentsRect());
    setGeometry(pr.center().x() - pr.width() / 4, pr.center().y() - pr.height() / 4,
                pr.width() / 2, pr.height() / 2);



    // load all algorithms on first launch to get the names
    if (mAlgorithmNames.size() == 0) {
        for (auto &desc : CalculationManager::getSingleton().getAlgorithms()) {
            const std::string &name = desc.first;
            try {
                auto info(std::move(CalculationManager::getSingleton().loadAlgorithmInformation(name)));
                mAlgorithmNames.push_back(qMakePair(QString::fromStdString(name), QString::fromStdString(info->getName())));
            } catch (...) {
                LogW("Error during loading and adding the algorithm '%s'. Skipping...", name.c_str());
            }
        }
    }

    // create default layout,
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    // title
    QHBoxLayout *titleLayout = new QHBoxLayout;
    mainLayout->addLayout(titleLayout);
    titleLayout->addWidget(new QLabel(tr("Algorithm:")));

    QComboBox *comboBox = new QComboBox;
    mAlgorithmSelection = comboBox;
    for (const auto &alg : mAlgorithmNames) {
        comboBox->addItem(alg.second, alg.first);
    }
    titleLayout->addWidget(comboBox);
    // select current algorithm
    comboBox->setCurrentIndex(comboBox->findData(QString::fromStdString(currentAlgorithm->getId())));
    EptAssert(comboBox->currentIndex() >= 0, "The default algotihm doesnt exist!");

    QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(algorithmSelectionChanged(int)));

    // scroll area
    mAlgorithmDescriptionScrollArea = new QScrollArea;
    mainLayout->addWidget(mAlgorithmDescriptionScrollArea);
    //mAlgorithmDescriptionScrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    mAlgorithmDescriptionScrollArea->setWidgetResizable(true);
    //mAlgorithmDescriptionScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //mAlgorithmDescriptionScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QScroller::grabGesture(mAlgorithmDescriptionScrollArea->viewport(), QScroller::LeftMouseButtonGesture);



    // button box
    QDialogButtonBox *buttons = new QDialogButtonBox;
    mainLayout->addWidget(buttons);
    buttons->setStandardButtons(QDialogButtonBox::Ok);

    QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    algorithmSelectionChanged(comboBox->currentIndex());

    SHOW_DIALOG(this);
}

void AlgorithmDialog::acceptCurrent() {
    if (!mCurrentFactoryDescription) {
        // nothin selected
        return;
    }

    // update the parameters
    for (auto &paramWidget : mAlgorithmWidgetConnectionList) {
        const AlgorithmParameter &param = mCurrentAlgorithmInformation->getParameter(paramWidget.first);
        const QWidget *widget = paramWidget.second;

        if (param.getType() == AlgorithmParameter::TYPE_DOUBLE) {
            const QDoubleSpinBox *sb = dynamic_cast<const QDoubleSpinBox*>(widget);
            EptAssert(sb, "This parameter is described by a QDoubleSpinBox");
            mCurrentFactoryDescription->setDoubleParameter(paramWidget.first, sb->value());
        } else if (param.getType() == AlgorithmParameter::TYPE_INT) {
            const QSpinBox *sb = dynamic_cast<const QSpinBox*>(widget);
            EptAssert(sb, "This parameter is described by a QSpinBox");
            mCurrentFactoryDescription->setIntParameter(paramWidget.first, sb->value());
        } else if (param.getType() == AlgorithmParameter::TYPE_LIST) {
            const QComboBox *cb = qobject_cast<const QComboBox*>(widget);
            mCurrentFactoryDescription->setStringParameter(paramWidget.first, cb->currentData().toString().toStdString());
        } else {
            EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Parameter type not implemented");
        }
    }

}

void AlgorithmDialog::algorithmSelectionChanged(int index) {
    // cleanup and save old
    acceptCurrent();
    if (mAlgorithmDescriptionScrollArea->widget()) {delete mAlgorithmDescriptionScrollArea->widget();}
    mAlgorithmWidgetConnectionList.clear();

    // load new
    QString algId = mAlgorithmSelection->itemData(index).toString();
    AlgorithmFactoryDescription &description = CalculationManager::getSingleton().getAlgorithmDescription(algId.toStdString());
    mCurrentFactoryDescription = &description;
    mCurrentAlgorithmInformation = CalculationManager::getSingleton().loadAlgorithmInformation(description.getAlgorithmName());
    const AlgorithmInformation &info(*mCurrentAlgorithmInformation.get());

    setWindowTitle(tr("Info of algorithm: %1").arg(QString::fromStdString(info.getName())));

    QWidget *scrollContentWidget = new QWidget;
    scrollContentWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mAlgorithmDescriptionScrollArea->setWidget(scrollContentWidget);
    QVBoxLayout *scrollLayout = new QVBoxLayout;
    scrollContentWidget->setLayout(scrollLayout);

    QGroupBox *infoGroupBox = new QGroupBox(tr("Info"));
    scrollLayout->addWidget(infoGroupBox);

    QFormLayout *layout = new QFormLayout;
    infoGroupBox->setLayout(layout);

    layout->addRow(new QLabel(tr("Name:")), new QLabel(QString::fromStdString(info.getName())));
    layout->addRow(new QLabel(tr("Author:")), new QLabel(QString::fromStdString(info.getAuthor())));
    layout->addRow(new QLabel(tr("Year:")), new QLabel(QString("%1").arg(info.getYear())));

    QLabel *descriptionLabel = new QLabel(QString::fromStdString(info.getDescription()));
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignTop);
    layout->addRow(new QLabel(tr("Description:")), descriptionLabel);


    // add parameters
    if (info.getParameters().size() > 0) {
        QGroupBox *paramsBox = new QGroupBox(tr("Parameters"));
        scrollLayout->addWidget(paramsBox);
        QFormLayout *paramsBoxLayout = new QFormLayout;
        paramsBox->setLayout(paramsBoxLayout);
        for (const AlgorithmParameter &param : info.getParameters()) {
            QWidget *dataWidget = nullptr;
            QLayout *dataLayout = nullptr;

            if (param.getType() == AlgorithmParameter::TYPE_DOUBLE) {
                QHBoxLayout *valueLayout = new QHBoxLayout;
                dataLayout = valueLayout;

                QDoubleSpinBox *sb = new QDoubleSpinBox();
                sb->setRange(param.getDoubleMinValue(), param.getDoubleMaxValue());
                if (description.hasDoubleParameter(param.getID())) {
                    sb->setValue(description.getDoubleParameter(param.getID()));
                } else {
                    sb->setValue(param.getDoubleDefaultValue());
                }

                if (param.getDoublePrecision() >= 0) {
                    // add a slider aswell
                    DoubleSlider *slider = new DoubleSlider(param.getDoubleMinValue(), param.getDoubleMaxValue(), param.getDoublePrecision());
                    slider->setOrientation(Qt::Horizontal);
                    slider->setValue(sb->value());
                    valueLayout->addWidget(slider);

                    QObject::connect(slider, SIGNAL(valueChanged(double)), sb, SLOT(setValue(double)));
                    QObject::connect(sb, SIGNAL(valueChanged(double)), slider, SLOT(setValue(double)));

                    sb->setDecimals(param.getDoublePrecision());
                }

                valueLayout->addWidget(sb);

                dataWidget = sb;
            } else if (param.getType() == AlgorithmParameter::TYPE_INT) {
                QHBoxLayout *valueLayout = new QHBoxLayout;
                dataLayout = valueLayout;

                QSpinBox *sb = new QSpinBox();
                sb->setRange(param.getIntMinValue(), param.getIntMaxValue());
                if (description.hasIntParameter(param.getID())) {
                    sb->setValue(description.getIntParameter(param.getID()));
                } else {
                    sb->setValue(param.getIntDefaultValue());
                }

                // add a slider aswell
                QSlider *slider = new QSlider(Qt::Horizontal);
                slider->setMinimum(param.getIntMinValue());
                slider->setMaximum(param.getIntMaxValue());
                slider->setValue(sb->value());
                valueLayout->addWidget(slider);

                QObject::connect(slider, SIGNAL(valueChanged(int)), sb, SLOT(setValue(int)));
                QObject::connect(sb, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));

                valueLayout->addWidget(sb);

                dataWidget = sb;
            } else if (param.getType() == AlgorithmParameter::TYPE_LIST) {
                QComboBox *cb = new QComboBox;
                for (const auto &p : param.getStringList()) {
                    cb->addItem(QString::fromStdString(p.second), QString::fromStdString(p.first));
                }
                int defaultIndex = cb->findData(QString::fromStdString(param.getStringDefaultValue()));
                if (description.hasStringParameter(param.getID())) {
                    defaultIndex = cb->findData(QString::fromStdString(description.getStringParameter(param.getID())));
                }
                cb->setCurrentIndex(defaultIndex);

                dataWidget = cb;
            } else {
                EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Parameter type not implemented.");
            }

            EptAssert(dataWidget, "A data widget has to exist.");

            dataWidget->setWhatsThis(QString::fromStdString(param.getDescription()));

            if (dataLayout) {
                paramsBoxLayout->addRow(new QLabel(QString::fromStdString(param.getLabel())), dataLayout);
            } else if (dataWidget) {
                paramsBoxLayout->addRow(new QLabel(QString::fromStdString(param.getLabel())), dataWidget);
            } else {
                EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "the parameter has to create a data layout or a data widget. Maybe it is not implemented at all");
            }

            mAlgorithmWidgetConnectionList.append(qMakePair(param.getID(), dataWidget));
        }
    }


    //layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
}

void AlgorithmDialog::accept() {
    QDialog::accept();

    acceptCurrent();
}
