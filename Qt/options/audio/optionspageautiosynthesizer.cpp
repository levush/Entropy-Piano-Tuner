#include "optionspageautiosynthesizer.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "settingsforqt.h"

namespace options {

PageAudioSynthesizer::PageAudioSynthesizer(OptionsDialog *optionsDialog, AudioPlayerAdapter *audioPlayer) :
    mAudioPlayer(audioPlayer) {
    QFormLayout *inputLayout = new QFormLayout;
    this->setLayout(inputLayout);

    // Channels
    QHBoxLayout *channelsLayout = new QHBoxLayout;
    inputLayout->addRow(new QLabel(tr("Channels")), channelsLayout);

    mChannelsSelect = new QComboBox();
    mChannelsSelect->addItem(QString::number(1), 1);
    mChannelsSelect->addItem(QString::number(2), 2);
    mChannelsSelect->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    mChannelsSelect->setCurrentText(QString::number(SettingsForQt::getSingleton().getAudioPlayerChannelsCount()));

    QPushButton *defaultChannels = new QPushButton(tr("Default"));
    QObject::connect(defaultChannels, SIGNAL(clicked(bool)), this, SLOT(onDefaultChannel()));

    channelsLayout->addWidget(mChannelsSelect);
    channelsLayout->addWidget(defaultChannels);

    // Buffer size
    QHBoxLayout *bufferSizeLayout = new QHBoxLayout;
    inputLayout->addRow(new QLabel(tr("Buffer size")), bufferSizeLayout);

    mBufferSizeEdit = new QSpinBox();
    mBufferSizeEdit->setRange(10, 500);
    mBufferSizeEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    mBufferSizeEdit->setValue(static_cast<int>(SettingsForQt::getSingleton().getAudioPlayerBufferSize() + 0.5));

    QPushButton *defaultBufferSize = new QPushButton(tr("Default"));
    QObject::connect(defaultBufferSize, SIGNAL(clicked(bool)), this, SLOT(onDefaultBufferSize()));

    bufferSizeLayout->addWidget(mBufferSizeEdit);
    bufferSizeLayout->addWidget(new QLabel(tr("ms")));
    bufferSizeLayout->addWidget(defaultBufferSize);



    // notify if changes are made
    QObject::connect(mChannelsSelect, SIGNAL(currentIndexChanged(int)), optionsDialog, SLOT(onChangesMade()));
    QObject::connect(mBufferSizeEdit, SIGNAL(valueChanged(int)), optionsDialog, SLOT(onChangesMade()));
}

void PageAudioSynthesizer::apply() {
    const int bufferSize = mBufferSizeEdit->value();
    const int channels = mChannelsSelect->currentData().toInt();

    SettingsForQt::getSingleton().setAudioPlayerBufferSize(bufferSize);
    SettingsForQt::getSingleton().setAudioPlayerChannelsCount(channels);

    // backup writer
    PCMWriterInterface *writer = mAudioPlayer->getWriter();
    mAudioPlayer->exit();

    mAudioPlayer->setChannelCount(channels);
    mAudioPlayer->setBufferSize(bufferSize);

    mAudioPlayer->init();
    mAudioPlayer->setWriter(writer);
}

void PageAudioSynthesizer::onDefaultChannel() {
    mChannelsSelect->setCurrentText(QString::number(2));
}

void PageAudioSynthesizer::onDefaultBufferSize() {
    mBufferSizeEdit->setValue(AudioPlayerAdapter::DefaultBufferSizeMilliseconds);
}

}  // namespace midi

