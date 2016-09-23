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

#ifndef OPTIONSPAGEAUDIOMIDIPAGE_H
#define OPTIONSPAGEAUDIOMIDIPAGE_H

#include <QWidget>
#include <QComboBox>

#include "prerequisites.h"

#include "core/audio/midi/midiadapter.h"

#include "../optionscontentswidgetinterface.h"

namespace options {

class PageAudioMidi
        : public QWidget
        , public ContentsWidgetInterface
        , public midi::MidiManagerListener {
    Q_OBJECT
public:
    PageAudioMidi(OptionsDialog *optionsDialog, MidiAdapter *midiInterface);

    void apply() override final;

protected:
    void updateMidiInputDevices();

    virtual void inputDeviceAttached(midi::MidiDeviceID id) override {MIDI_UNUSED(id); updateMidiInputDevices();}
    virtual void inputDeviceDetached(midi::MidiDeviceID id) override {MIDI_UNUSED(id); updateMidiInputDevices();}

    virtual void inputDeviceCreated(midi::MidiInputDevicePtr device) override {MIDI_UNUSED(device); updateMidiInputDevices();}
    virtual void inputDeviceDeleted(midi::MidiDeviceID id) override {MIDI_UNUSED(id); updateMidiInputDevices();}
private:
    MidiAdapter *mMidiInterface;

    QComboBox *mDeviceSelection;
};


}  // namespace options

#endif // OPTIONSPAGEAUDIOMIDIPAGE_H
