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

package org.uniwue.tp3;

import android.content.Context;
import android.widget.Toast;
import android.hardware.usb.UsbDevice;
import android.util.Log;


import jp.kshoji.driver.midi.device.MidiInputDevice;
import jp.kshoji.driver.midi.device.MidiOutputDevice;
import jp.kshoji.driver.midi.util.UsbMidiDriver;

public class UsbMidiDriverAdapter {
    private UsbMidiDriver usbMidiDriver;

    public static Context mContext;

    public void create(Context context) {
        mContext = context;
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
            public void onMidiReset(MidiInputDevice midiInputDevice, int id) {
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
                TunerApplication.java_sendMidiMessage(2, note, velocity, 0);
            }

            @Override
            public void onMidiNoteOn(final MidiInputDevice sender, int cable, int channel, int note, int velocity) {
                TunerApplication.java_sendMidiMessage(1, note, velocity, 0);
            }

            @Override
            public void onMidiPolyphonicAftertouch(final MidiInputDevice sender, int cable, int channel, int note, int pressure) {
            }

            @Override
            public void onMidiControlChange(final MidiInputDevice sender, int cable, int channel, int function, int value) {
                TunerApplication.java_sendMidiMessage(3, function, value, 0);
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

            @Override
            public void onMidiActiveSensing(MidiInputDevice device, int cable) {
            }

            @Override
            public void onMidiStop(MidiInputDevice device, int cable) {
            }

            @Override
            public void onMidiContinue(MidiInputDevice device, int cable) {
            }

            @Override
            public void onMidiStart(MidiInputDevice device, int cable) {
            }

            @Override
            public void onMidiTimingClock(MidiInputDevice device, int cable) {
            }

            @Override
            public void onMidiTuneRequest(MidiInputDevice device, int cable) {
            }

            @Override
            public void onMidiSongPositionPointer(MidiInputDevice device, int cable, int byte1) {
            }

            @Override
            public void onMidiSongSelect(MidiInputDevice device, int cable, int byte1) {
            }

            @Override
            public void onMidiTimeCodeQuarterFrame(MidiInputDevice device, int cable, int byte1) {
            }

        };

        usbMidiDriver.open();

    }

    public void destroy() {
        usbMidiDriver.close();
        usbMidiDriver = null;
    }

}

