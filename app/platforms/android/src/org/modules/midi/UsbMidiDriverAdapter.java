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


 package org.modules.midi;

 import android.content.Context;
 import android.widget.Toast;
 import android.hardware.usb.UsbDevice;
 import android.util.Log;
 import android.text.TextUtils;
 import java.util.List;
 import java.util.ArrayList;

 import jp.kshoji.driver.midi.device.MidiInputDevice;
 import jp.kshoji.driver.midi.device.MidiOutputDevice;
 import jp.kshoji.driver.midi.util.UsbMidiDriver;

 public class UsbMidiDriverAdapter {
     private static UsbMidiDriverAdapter mUsbMidiDriverAdapter;
     private UsbMidiDriver usbMidiDriver;

     public static Context mContext;

     private boolean mActiveInput = false;
     private boolean mActiveOutput = false;

     private List<MidiInputDevice> mInputDevices = new ArrayList<MidiInputDevice>();
     private List<MidiOutputDevice> mOutputDevices = new ArrayList<MidiOutputDevice>();

     // Native methods
     public static native void java_midi_sendMidiMessage(String deviceName, int event, int byte1, int byte2);
     public static native void java_midi_inputDeviceAttached(String deviceName);
     public static native void java_midi_inputDeviceDetached(String deviceName);
     public static native void java_midi_outputDeviceAttached(String deviceName);
     public static native void java_midi_outputDeviceDetached(String deviceName);

     public UsbMidiDriverAdapter() {
         mUsbMidiDriverAdapter = this;
     }

     public static UsbMidiDriverAdapter instance() {
         return mUsbMidiDriverAdapter;
     }

     public void create(Context context) {
         mContext = context;
         usbMidiDriver = new UsbMidiDriver(context) {
             @Override
             public void onDeviceAttached(UsbDevice usbDevice) {
                 // deprecated method.
             }

             @Override
             public void onMidiInputDeviceAttached(MidiInputDevice midiInputDevice) {
                 mInputDevices.add(midiInputDevice);
                 java_midi_inputDeviceAttached(getDeviceName(midiInputDevice));
             }

             @Override
             public void onMidiOutputDeviceAttached(final MidiOutputDevice midiOutputDevice) {
                 mOutputDevices.add(midiOutputDevice);
                 java_midi_outputDeviceAttached(getDeviceName(midiOutputDevice));
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
                 mInputDevices.remove(midiInputDevice);

                 java_midi_inputDeviceDetached(getDeviceName(midiInputDevice));
             }

             @Override
             public void onMidiOutputDeviceDetached(final MidiOutputDevice midiOutputDevice) {
                 mOutputDevices.remove(midiOutputDevice);

                 java_midi_outputDeviceDetached(getDeviceName(midiOutputDevice));
             }

             @Override
             public void onMidiNoteOff(final MidiInputDevice sender, int cable, int channel, int note, int velocity) {
                 java_midi_sendMidiMessage(getDeviceName(sender), 0x80, note, velocity);
             }

             @Override
             public void onMidiNoteOn(final MidiInputDevice sender, int cable, int channel, int note, int velocity) {
                 java_midi_sendMidiMessage(getDeviceName(sender), 0x90, note, velocity);
             }

             @Override
             public void onMidiPolyphonicAftertouch(final MidiInputDevice sender, int cable, int channel, int note, int pressure) {
                 java_midi_sendMidiMessage(getDeviceName(sender), 0xA0, note, pressure);
             }

             @Override
             public void onMidiControlChange(final MidiInputDevice sender, int cable, int channel, int function, int value) {
                 java_midi_sendMidiMessage(getDeviceName(sender), 0xB0, function, value);
             }

             @Override
             public void onMidiProgramChange(final MidiInputDevice sender, int cable, int channel, int program) {
                 java_midi_sendMidiMessage(getDeviceName(sender), 0xC0, program, 0);
             }

             @Override
             public void onMidiChannelAftertouch(final MidiInputDevice sender, int cable, int channel, int pressure) {
                 java_midi_sendMidiMessage(getDeviceName(sender), 0xD0, pressure, 0);
             }

             @Override
             public void onMidiPitchWheel(final MidiInputDevice sender, int cable, int channel, int amount) {
                 java_midi_sendMidiMessage(getDeviceName(sender), 0xE0, amount, 0);
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


     private void suspendAllInputDevices() {
         for (MidiInputDevice dev : mInputDevices) {
             dev.suspend();
         }
     }

     private void suspendAllOutputDevices() {
         for (MidiOutputDevice dev : mOutputDevices) {
             dev.suspend();
         }
     }

     public void init() {
         usbMidiDriver.open();
     }

     public void exit() {
         usbMidiDriver.close();
     }

     public void initInput() {
         mActiveInput = true;
         init();
     }

     public void exitInput() {
         mActiveInput = false;
         if (!mActiveInput && !mActiveOutput) {
             exit();
         }
     }

     public void initOutput() {
         mActiveOutput = true;
         init();
     }

     public void exitOutput() {
         mActiveOutput = false;
         if (!mActiveInput && !mActiveOutput) {
             exit();
         }
     }


     public boolean connectInputDevice(String name) {
         MidiInputDevice dev = getInputDeviceByName(name);
         if (dev != null) {
             dev.resume();
             return true;
         } else {
             return false;
         }
     }

     public boolean disconnectInputDevice(String name) {
         MidiInputDevice device = getInputDeviceByName(name);
         if (device == null) {return false;}
         device.suspend();
         return true;
     }

     public boolean connectOutputDevice(String name) {
         MidiOutputDevice dev = getOutputDeviceByName(name);
         if (dev != null) {
             dev.resume();
             return true;
         } else {
             return false;
         }
     }

     public boolean disconnectOutputDevice(String name) {
         MidiOutputDevice device = getOutputDeviceByName(name);
         if (device == null) {return false;}
         device.suspend();
         return true;
     }

     public String getDeviceName(MidiInputDevice device) {
         if  (device == null) {return "";}
         String s = device.getProductName();
         if (s != null) {return s;}
         s = device.getManufacturerName();
         if (s != null) {return s;}
         return device.getDeviceAddress();
     }

     public String getDeviceName(MidiOutputDevice device) {
         if  (device == null) {return "";}
         String s = device.getProductName();
         if (s != null) {return s;}
         s = device.getManufacturerName();
         if (s != null) {return s;}
         return device.getDeviceAddress();
     }

     public String getInputDeviceNames() {
         List<String> names = new ArrayList<String>();
         for (int i = 0; i < mInputDevices.size(); ++i) {
             names.add(getDeviceName(mInputDevices.get(i)));
         }

         return TextUtils.join("\n", names);

     }

     public String getOutputDeviceNames() {
         List<String> names = new ArrayList<String>();
         for (int i = 0; i < mOutputDevices.size(); ++i) {
             names.add(getDeviceName(mOutputDevices.get(i)));
         }

         return TextUtils.join("\n", names);

     }

     public MidiInputDevice getInputDeviceByName(String s) {
         for (MidiInputDevice dev : mInputDevices) {
             if (getDeviceName(dev).equals(s)) {
                 return dev;
             }
         }

         return null;
     }

     public MidiOutputDevice getOutputDeviceByName(String s) {
         for (MidiOutputDevice dev : mOutputDevices) {
             if (getDeviceName(dev).equals(s)) {
                 return dev;
             }
         }

         return null;
     }


     public void receiveMidiOutputEvent(String s, int byte1, int byte2, int byte3) {
         MidiOutputDevice dev = getOutputDeviceByName(s);
         if (dev != null) {
             dev.sendMidiMessage(0, byte1, byte2, byte3);
         }
     }

}


