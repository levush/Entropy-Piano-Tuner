#ifndef OPTIONSPAGEAUTIOSYNTHESIZER_H
#define OPTIONSPAGEAUTIOSYNTHESIZER_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include "core/audio/player/audioplayeradapter.h"
#include "../optionscontentswidgetinterface.h"

namespace options {

class PageAudioSynthesizer : public QWidget, public ContentsWidgetInterface {
    Q_OBJECT
public:
    PageAudioSynthesizer(OptionsDialog *optionsDialog, AudioPlayerAdapter *audioPlayer);

    void apply() override final;

private slots:
    void onDefaultChannel();
    void onDefaultBufferSize();

private:
    AudioPlayerAdapter *mAudioPlayer;

    QComboBox *mChannelsSelect;
    QSpinBox *mBufferSizeEdit;
};


}  // namespace options

#endif // OPTIONSPAGEAUTIOSYNTHESIZER_H
