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
#include <QScrollBar>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>

#include "core/config.h"
#include "core/calculation/algorithmfactorydescription.h"
#include "core/calculation/calculationmanager.h"
#include "core/calculation/algorithminformation.h"
#include "core/piano/piano.h"

#include "widgets/doubleslider.h"
#include "algorithmdialogparameterupdatetimer.h"
#include "displaysize.h"
#include "qtconfig.h"

AlgorithmDialog::AlgorithmIdList AlgorithmDialog::mAlgorithmNames;

AlgorithmDialog::AlgorithmDialog(std::shared_ptr<const AlgorithmInformation> currentAlgorithm, Piano &piano, QWidget *parent) :
    QDialog(parent),
    mPiano(piano)
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
                auto info(CalculationManager::getSingleton().loadAlgorithmInformation(name));
                mAlgorithmNames.push_back(qMakePair(QString::fromStdString(name), QString::fromStdWString(info->getName())));
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
    mAlgorithmDescriptionScrollArea->setWidgetResizable(true);
    //mAlgorithmDescriptionScrollArea->setFrameShape(QFrame::NoFrame);
    mainLayout->addWidget(mAlgorithmDescriptionScrollArea);
    mAlgorithmDescriptionScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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

bool AlgorithmDialog::eventFilter(QObject *o, QEvent *e) {
    if (o == mAlgorithmDescriptionScrollArea->widget() && e->type() == QEvent::Resize) {
        QWidget *contents = mAlgorithmDescriptionScrollArea->widget();
        QScrollBar *vScrollBar = mAlgorithmDescriptionScrollArea->verticalScrollBar();

        setMinimumWidth(contents->minimumSizeHint().width()
                        + vScrollBar->width()
                        + contents->layout()->margin() * 2);
    }

    return false;
}

void AlgorithmDialog::acceptCurrent() {
    if (!mCurrentAlgorithmParameters) {
        // nothin selected
        return;
    }

    // update the parameters
    for (auto &paramWidget : mAlgorithmWidgetConnectionList) {
        const AlgorithmParameterDescription &param = mCurrentAlgorithmInformation->getParameter(paramWidget.first);
        const QWidget *widget = paramWidget.second;

        if (param.getType() == AlgorithmParameterDescription::TYPE_DOUBLE) {
            if (param.displaySpinBox()) {
                const QDoubleSpinBox *sb = dynamic_cast<const QDoubleSpinBox*>(widget);
                EptAssert(sb, "This parameter is described by a QDoubleSpinBox");
                mCurrentAlgorithmParameters->setDoubleParameter(paramWidget.first, sb->value());
            } else if (param.displayLineEdit()) {
                const QLineEdit *le = dynamic_cast<const QLineEdit*>(widget);
                EptAssert(le, "This parameter is described by a QLineEdit");
                mCurrentAlgorithmParameters->setDoubleParameter(paramWidget.first, le->text().toDouble());
            } else {
                EPT_EXCEPT(EptException::ERR_INVALIDPARAMS, "Either display spin box or line edit have to be set.");
            }
        } else if (param.getType() == AlgorithmParameterDescription::TYPE_INT) {
            if (param.displaySpinBox()) {
                const QSpinBox *sb = dynamic_cast<const QSpinBox*>(widget);
                EptAssert(sb, "This parameter is described by a QSpinBox");
                mCurrentAlgorithmParameters->setIntParameter(paramWidget.first, sb->value());
            } else if (param.displayLineEdit()) {
                const QLineEdit *le = dynamic_cast<const QLineEdit*>(widget);
                EptAssert(le, "This parameter is described by a QLineEdit");
                mCurrentAlgorithmParameters->setIntParameter(paramWidget.first, le->text().toInt());
            } else {
                EPT_EXCEPT(EptException::ERR_INVALIDPARAMS, "Either display spin box or line edit have to be set.");
            }
        } else if (param.getType() == AlgorithmParameterDescription::TYPE_LIST) {
            const QComboBox *cb = qobject_cast<const QComboBox*>(widget);
            mCurrentAlgorithmParameters->setStringParameter(paramWidget.first, cb->currentData().toString().toStdString());
        } else if (param.getType() == AlgorithmParameterDescription::TYPE_BOOL) {
            const QCheckBox *cb = qobject_cast<const QCheckBox*>(widget);
            mCurrentAlgorithmParameters->setBoolParameter(paramWidget.first, cb->isChecked());
        } else {
            EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Parameter type not implemented");
        }
    }

}

void AlgorithmDialog::algorithmSelectionChanged(int index) {
    // cleanup and save old
    acceptCurrent();
    if (mAlgorithmDescriptionScrollArea->widget()) {delete mAlgorithmDescriptionScrollArea->widget();}
    mAlgorithmDescriptionScrollArea->setWidget(nullptr);
    mAlgorithmWidgetConnectionList.clear();

    auto applyFormLayoutFormat = [](QFormLayout *l) {
        if (DisplaySizeDefines::getSingleton()->showMultiLineEditPianoDataSheet()) {
            l->setRowWrapPolicy(QFormLayout::WrapAllRows);
        } else {
            l->setRowWrapPolicy(QFormLayout::DontWrapRows);
        }
    };

    // load new
    QString algId = mAlgorithmSelection->itemData(index).toString();
    mCurrentAlgorithmParameters = mPiano.getAlgorithmParameters().getOrCreate(algId.toStdString());
    mCurrentAlgorithmInformation = CalculationManager::getSingleton().loadAlgorithmInformation(mCurrentAlgorithmParameters->getAlgorithmName());
    const AlgorithmInformation &info(*mCurrentAlgorithmInformation.get());
    const SingleAlgorithmParameters &description = *mCurrentAlgorithmParameters;

    setWindowTitle(tr("Info of algorithm: %1").arg(QString::fromStdWString(info.getName())));

    QWidget *scrollContentWidget = new QWidget;
    scrollContentWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    scrollContentWidget->installEventFilter(this);
    QVBoxLayout *scrollLayout = new QVBoxLayout;
    scrollContentWidget->setLayout(scrollLayout);

    mAlgorithmDescriptionScrollArea->setWidget(scrollContentWidget);

    QGroupBox *infoGroupBox = new QGroupBox(tr("Info"));
    scrollLayout->addWidget(infoGroupBox);

    QFormLayout *layout = new QFormLayout;
    infoGroupBox->setLayout(layout);
    //applyFormLayoutFormat(layout);

    layout->addRow(new QLabel(tr("Name:")), new QLabel(QString::fromStdWString(info.getName())));
    layout->addRow(new QLabel(tr("Version:")), new QLabel(QString::fromStdString(info.getVersion())));
    layout->addRow(new QLabel(tr("Author:")), new QLabel(QString::fromStdWString(info.getAuthor())));
    layout->addRow(new QLabel(tr("Year:")), new QLabel(QString::fromStdWString(info.getYear())));

    QLabel *descriptionLabel = new QLabel(QString::fromStdWString(info.getDescription()));
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignTop);
    layout->addRow(new QLabel(tr("Description:")), descriptionLabel);


    // add parameters
    if (info.getParameters().size() > 0) {
        QGroupBox *paramsBox = new QGroupBox(tr("Parameters"));
        scrollLayout->addWidget(paramsBox);
        QFormLayout *paramsBoxLayout = new QFormLayout;
        paramsBox->setLayout(paramsBoxLayout);
        applyFormLayoutFormat(paramsBoxLayout);
        for (const AlgorithmParameterDescription &param : info.getParameters()) {
            QWidget *dataWidget = nullptr;
            QHBoxLayout *dataLayout = new QHBoxLayout;

            EptAssert((param.displayLineEdit() && !param.displaySpinBox())
                      || (!param.displayLineEdit() && param.displaySpinBox()),
                      "Either line edit or spin box may be enabled for a parameter");

            if (param.getType() == AlgorithmParameterDescription::TYPE_DOUBLE) {
                double value = param.getDoubleDefaultValue();
                if (description.hasDoubleParameter(param.getID())) {
                    value = description.getDoubleParameter(param.getID());
                }

                QLineEdit *le = nullptr;
                QDoubleSpinBox *sb = nullptr;
                DoubleSlider *slider = nullptr;

                if (param.displayLineEdit()) {
                    le = new QLineEdit();
                    le->setReadOnly(param.readOnly());
                    le->setValidator(new QDoubleValidator(param.getDoubleMinValue(), param.getDoubleMaxValue(), param.getDoublePrecision()));
                    le->setText(QString::number(value));
                    dataLayout->addWidget(le);
                    dataWidget = le;
                }

                if (param.displaySpinBox()) {
                    sb = new QDoubleSpinBox();
                    sb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                    sb->setReadOnly(param.readOnly());
#ifdef __ANDROID__
                    // HACK: fix spin box size on android
                    sb->setMinimumWidth(sb->fontInfo().pointSizeF() * 15);
  #endif
                    sb->setRange(param.getDoubleMinValue(), param.getDoubleMaxValue());
                    sb->setDecimals(param.getDoublePrecision());
                    sb->setValue(value);
                    dataLayout->addWidget(sb);
                    dataWidget = sb;
                }

                if (param.getDoublePrecision() >= 0 && param.displaySlider()) {
                    // add a slider aswell
                    slider = new DoubleSlider(param.getDoubleMinValue(), param.getDoubleMaxValue(), param.getDoublePrecision());
                    slider->setOrientation(Qt::Horizontal);
                    slider->setValue(sb->value());
                }

                if (slider) {
                    dataLayout->addWidget(slider);

                    if (le) {
                        QObject::connect(slider, SIGNAL(valueChanged(QString)), le, SLOT(setText(QString)));
                        QObject::connect(le, SIGNAL(textChanged(QString)), slider, SLOT(setValue(QString)));
                    }

                    if (sb) {
                        QObject::connect(slider, SIGNAL(valueChanged(double)), sb, SLOT(setValue(double)));
                        QObject::connect(sb, SIGNAL(valueChanged(double)), slider, SLOT(setValue(double)));
                    }
                }
            } else if (param.getType() == AlgorithmParameterDescription::TYPE_INT) {
                int value = param.getIntDefaultValue();
                if (description.hasIntParameter(param.getID())) {
                    value = description.getIntParameter(param.getID());
                }

                QLineEdit *le = nullptr;
                QSpinBox *sb = nullptr;
                DoubleSlider *slider = nullptr;

                if (param.displayLineEdit()) {
                    le = new QLineEdit();
                    le->setReadOnly(param.readOnly());
                    le->setValidator(new QIntValidator(param.getIntMinValue(), param.getIntMaxValue()));
                    le->setText(QString::number(value));
                    dataLayout->addWidget(le);
                    dataWidget = le;
                }

                if (param.displaySpinBox()) {
                    sb = new QSpinBox();
                    sb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                    sb->setReadOnly(param.readOnly());
#ifdef __ANDROID__
                    // HACK: fix spin box size on android
                    sb->setMinimumWidth(sb->fontInfo().pointSizeF() * 15);
#endif
                    sb->setRange(param.getIntMinValue(), param.getIntMaxValue());
                    sb->setValue(value);
                    dataLayout->addWidget(sb);
                    dataWidget = sb;
                }

                // add a slider aswell if desired
                if (param.displaySlider()) {
                    QSlider *slider = new QSlider(Qt::Horizontal);
                    slider->setMinimum(param.getIntMinValue());
                    slider->setMaximum(param.getIntMaxValue());
                    slider->setValue(value);
                }

                if (slider) {
                    dataLayout->addWidget(slider);

                    if (le) {
                        QObject::connect(slider, SIGNAL(valueChanged(QString)), le, SLOT(setText(QString)));
                        QObject::connect(le, SIGNAL(textChanged(QString)), slider, SLOT(setValue(QString)));
                    }

                    if (sb) {
                        QObject::connect(slider, SIGNAL(valueChanged(double)), sb, SLOT(setValue(double)));
                        QObject::connect(sb, SIGNAL(valueChanged(double)), slider, SLOT(setValue(double)));
                    }
                }
            } else if (param.getType() == AlgorithmParameterDescription::TYPE_LIST) {
                QComboBox *cb = new QComboBox;
                cb->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
                dataLayout->addWidget(cb);

                for (const auto &p : param.getStringList()) {
                    cb->addItem(QString::fromStdWString(p.second), QString::fromStdString(p.first));
                }
                int defaultIndex = cb->findData(QString::fromStdString(param.getStringDefaultValue()));
                if (description.hasStringParameter(param.getID())) {
                    defaultIndex = cb->findData(QString::fromStdString(description.getStringParameter(param.getID())));
                }
                cb->setCurrentIndex(defaultIndex);

                dataWidget = cb;
            } else if (param.getType() == AlgorithmParameterDescription::TYPE_BOOL) {
                QCheckBox *cb = new QCheckBox;
                cb->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
                dataLayout->addWidget(cb);

                cb->setChecked(param.getBoolDefaultValue());
                if (description.hasBoolParameter(param.getID())) {
                    cb->setChecked(description.getBoolParameter(param.getID()));
                }

                dataWidget = cb;
            } else {
                EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Parameter type not implemented.");
            }

            EptAssert(dataWidget, "A data widget has to exist.");

            // create label
            QLabel *label = new QLabel(QString::fromStdWString(param.getLabel()));

            // apply tool tip and whats this
            dataWidget->setWhatsThis(QString::fromStdWString(param.getDescription()));
            dataWidget->setToolTip(dataWidget->whatsThis());
            label->setWhatsThis(dataWidget->whatsThis());
            label->setToolTip(dataWidget->toolTip());

            // create default button
            if (param.displaySetDefaultButton() && !param.readOnly()) {
                QPushButton *defaultButton = new DefaultButton(tr("Default"), param.getID(), dataWidget);
                defaultButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
                defaultButton->setWhatsThis(tr("Reset the parameter to its default value"));
                defaultButton->setToolTip(defaultButton->whatsThis());
                dataLayout->addWidget(defaultButton);
                QObject::connect(defaultButton, SIGNAL(clicked(bool)), this, SLOT(defaultButtonClicked()));
            }

            // create update timer
            if (param.updateIntervalInMS() >= 0) {
                new AlgorithmDialogParameterUpdateTimer(param, mCurrentAlgorithmParameters, dataWidget, scrollContentWidget);
            }

            paramsBoxLayout->addRow(label, dataLayout);

            mAlgorithmWidgetConnectionList.append(qMakePair(param.getID(), dataWidget));
        }
    }


    //layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
}

void AlgorithmDialog::defaultButtonClicked() {
    DefaultButton *defaultButton = dynamic_cast<DefaultButton*>(sender());
    EptAssert(defaultButton, "Sender is not a DefaultButton");


    QWidget *dataWidget = defaultButton->getDataWidget();
    const AlgorithmInformation::ParameterType &param = mCurrentAlgorithmInformation->getParameter(defaultButton->getID());

    switch (param.getType()) {
    case AlgorithmParameterDescription::TYPE_DOUBLE:
        if (param.displayLineEdit()) {
            dynamic_cast<QLineEdit*>(dataWidget)->setText(QString::number(param.getDoubleDefaultValue()));
        }
        if (param.displaySpinBox()) {
            dynamic_cast<QDoubleSpinBox*>(dataWidget)->setValue(param.getDoubleDefaultValue());
        }
        break;
    case AlgorithmParameterDescription::TYPE_INT:
        if (param.displayLineEdit()) {
            dynamic_cast<QLineEdit*>(dataWidget)->setText(QString::number(param.getIntDefaultValue()));
        }
        if (param.displaySpinBox()) {
            dynamic_cast<QSpinBox*>(dataWidget)->setValue(param.getIntDefaultValue());
        }
        break;
    case AlgorithmParameterDescription::TYPE_LIST: {
        QComboBox *cb = dynamic_cast<QComboBox*>(dataWidget);
        cb->setCurrentIndex(cb->findData(QString::fromStdString(param.getStringDefaultValue())));
        break; }
    case AlgorithmParameterDescription::TYPE_BOOL:
        dynamic_cast<QCheckBox*>(dataWidget)->setChecked(param.getBoolDefaultValue());
        break;
    default:
        EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Default button does not implement type.");
    }

}

void AlgorithmDialog::accept() {
    QDialog::accept();

    acceptCurrent();
}
