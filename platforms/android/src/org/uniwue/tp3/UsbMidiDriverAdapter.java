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

package org.uniwue.tp3;

import android.content.Context;
import android.hardware.usb.UsbDevice;
import android.util.Log;


import jp.kshoji.driver.midi.device.MidiInputDevice;
import jp.kshoji.driver.midi.device.MidiOutputDevice;
import jp.kshoji.driver.midi.util.UsbMidiDriver;

public class UsbMidiDriverAdapter {
    private UsbMidiDriver usbMidiDriver;

    public static native void java_sendMidiMessage(String s, boolean cached);

    public void create(Context context) {
        usbMidiDriver = new UsbMidiDriver(context) {
            @Override
            public void onDeviceAttached(UsbDevice usbDevice) {
                // deprecated method.
            }

            @Override
            public void onMidiInputDeviceAttached(MidiInputDevice midiInputDevice) {

            }

            @Override
            public void onMidiOutputDeviceAttached(final MidiOutputDevice midiOutputDevice) {
            }

            @Override
            public void onDeviceDetached(UsbDevice usbDevice) {
                // deprecated method.
            }

            @Override
            public void onMidiInputDeviceDetached(MidiInputDevice midiInputDevice) {

            }

            @Override
            public void onMidiOutputDeviceDetached(final MidiOutputDevice midiOutputDevice) {
            }

            @Override
            public void onMidiNoteOff(final MidiInputDevice sender, int cable, int channel, int note, int velocity) {
                Log.d("MIDI", "Key released");
            }

            @Override
            public void onMidiNoteOn(final MidiInputDevice sender, int cable, int channel, int note, int velocity) {
                Log.d("MIDI", "Key pressed");
            }

            @Override
            public void onMidiPolyphonicAftertouch(final MidiInputDevice sender, int cable, int channel, int note, int pressure) {
            }

            @Override
            public void onMidiControlChange(final MidiInputDevice sender, int cable, int channel, int function, int value) {
            }

            @Override
            public void onMidiProgramChange(final MidiInputDevice sender, int cable, int channel, int program) {
            }

            @Override
            public void onMidiChannelAftertouch(final MidiInputDevice sender, int cable, int channel, int pressure) {
            }

            @Override
            public void onMidiPitchWheel(final MidiInputDevice sender, int cable, int channel, int amount) {
            }

            @Override
            public void onMidiSystemExclusive(final MidiInputDevice sender, int cable, final byte[] systemExclusive) {
            }

            @Override
            public void onMidiSystemCommonMessage(final MidiInputDevice sender, int cable, final byte[] bytes) {
            }

            @Override
            public void onMidiSingleByte(final MidiInputDevice sender, int cable, int byte1) {
            }

            @Override
            public void onMidiMiscellaneousFunctionCodes(final MidiInputDevice sender, int cable, int byte1, int byte2, int byte3) {
            }

            @Override
            public void onMidiCableEvents(final MidiInputDevice sender, int cable, int byte1, int byte2, int byte3) {
            }
        };

        usbMidiDriver.open();

    }

    public void destroy() {
        usbMidiDriver.close();
        usbMidiDriver = null;
    }

}

