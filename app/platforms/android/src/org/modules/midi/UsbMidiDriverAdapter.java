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
     private MidiInputDevice mCurrentInputDevice = null;

     private List<MidiOutputDevice> mOutputDevices = new ArrayList<MidiOutputDevice>();
     private MidiOutputDevice mCurrentOutputDevice = null;

     // Native methods
     public static native void java_sendMidiMessage(String deviceName, int event, int byte1, int byte2);
     public static native void java_inputDeviceAttached(String deviceName);
     public static native void java_inputDeviceDetached(String deviceName);
     public static native void java_outputDeviceAttached(String deviceName);
     public static native void java_outputDeviceDetached(String deviceName);

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
                 java_inputDeviceAttached(getDeviceName(midiInputDevice));
             }

             @Override
             public void onMidiOutputDeviceAttached(final MidiOutputDevice midiOutputDevice) {
                 mOutputDevices.add(midiOutputDevice);
                 java_outputDeviceAttached(getDeviceName(midiOutputDevice));
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
                 java_inputDeviceDetached(getDeviceName(midiInputDevice));
                 if (midiInputDevice == mCurrentInputDevice) {
                     mCurrentInputDevice = null;
                 }

                 mInputDevices.remove(midiInputDevice);
             }

             @Override
             public void onMidiOutputDeviceDetached(final MidiOutputDevice midiOutputDevice) {
                 java_outputDeviceDetached(getDeviceName(midiOutputDevice));
                 if (midiOutputDevice == mCurrentOutputDevice) {
                     mCurrentOutputDevice = null;
                 }

                 mOutputDevices.remove(midiOutputDevice);
             }

             @Override
             public void onMidiNoteOff(final MidiInputDevice sender, int cable, int channel, int note, int velocity) {
                 if (mCurrentInputDevice == sender)
                     java_sendMidiMessage(getDeviceName(sender), 0x80, note, velocity);
             }

             @Override
             public void onMidiNoteOn(final MidiInputDevice sender, int cable, int channel, int note, int velocity) {
                 if (mCurrentInputDevice == sender)
                     java_sendMidiMessage(getDeviceName(sender), 0x90, note, velocity);
             }

             @Override
             public void onMidiPolyphonicAftertouch(final MidiInputDevice sender, int cable, int channel, int note, int pressure) {
                 if (mCurrentInputDevice == sender)
                     java_sendMidiMessage(getDeviceName(sender), 0xA0, note, pressure);
             }

             @Override
             public void onMidiControlChange(final MidiInputDevice sender, int cable, int channel, int function, int value) {
                 if (mCurrentInputDevice == sender)
                     java_sendMidiMessage(getDeviceName(sender), 0xB0, function, value);
             }

             @Override
             public void onMidiProgramChange(final MidiInputDevice sender, int cable, int channel, int program) {
                 if (mCurrentInputDevice == sender)
                     java_sendMidiMessage(getDeviceName(sender), 0xC0, program, 0);
             }

             @Override
             public void onMidiChannelAftertouch(final MidiInputDevice sender, int cable, int channel, int pressure) {
                 if (mCurrentInputDevice == sender)
                     java_sendMidiMessage(getDeviceName(sender), 0xD0, pressure, 0);
             }

             @Override
             public void onMidiPitchWheel(final MidiInputDevice sender, int cable, int channel, int amount) {
                 if (mCurrentInputDevice == sender)
                     java_sendMidiMessage(getDeviceName(sender), 0xE0, amount, 0);
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


     public void startInput() {
         if (mCurrentInputDevice != null) mCurrentInputDevice.resume();
     }

     public void startOutput() {
         if (mCurrentOutputDevice != null) mCurrentOutputDevice.resume();
     }

     public void stopInput() {
         suspendAllInputDevices();
     }

     public void stopOutput() {
         suspendAllOutputDevices();
     }

     public boolean isMidiInputInterfaceAvailable() {
         return mInputDevices.size() > 0;
     }

     public boolean isMidiOutputInterfaceAvailable() {
         return mOutputDevices.size() > 0;
     }

     public boolean connectInputDevice(String name) {
         suspendAllInputDevices();

         mCurrentInputDevice = getInputDeviceByName(name);
         if (mCurrentInputDevice == null) {return false;}
         mCurrentInputDevice.resume();
         return true;
     }

     public boolean disconnectInputDevice(String name) {
         MidiInputDevice device = getInputDeviceByName(name);
         if (device == null) {return false;}
         device.suspend();
         return true;
     }

     public boolean connectOutputDevice(String name) {
         suspendAllOutputDevices();

         mCurrentOutputDevice = getOutputDeviceByName(name);
         if (mCurrentOutputDevice == null) {return false;}
         mCurrentOutputDevice.resume();
         return true;
     }

     public boolean disconnectOutputDevice(String name) {
         MidiOutputDevice device = getOutputDeviceByName(name);
         if (device == null) {return false;}
         device.suspend();
         return true;
     }

     public boolean connectDefaultInputDevice() {
         suspendAllInputDevices();

         if (mInputDevices.size() == 0) {return false;}
         mCurrentInputDevice = mInputDevices.get(0);
         mCurrentInputDevice.resume();
         return true;
     }

     public boolean connectDefaultOutputDevice() {
         suspendAllOutputDevices();

         if (mOutputDevices.size() == 0) {return false;}
         mCurrentOutputDevice = mOutputDevices.get(0);
         mCurrentOutputDevice.resume();
         return true;
     }

     public void disconnectInputDevice() {
         if (mCurrentInputDevice != null) {
             mCurrentInputDevice.suspend();
             mCurrentInputDevice = null;
         }
     }

     public void disconnectOutputDevice() {
         if (mCurrentOutputDevice != null) {
             mCurrentOutputDevice.suspend();
             mCurrentOutputDevice = null;
         }
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

     public String getCurrentInputDeviceName() {
         return getDeviceName(mCurrentInputDevice);
     }

     public String getCurrentOutputDeviceName() {
         return getDeviceName(mCurrentOutputDevice);
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


     public void receiveMidiOutputEvent(int byte1, int byte2, int byte3) {
         if (mCurrentOutputDevice != null) {
             mCurrentOutputDevice.sendMidiMessage(0, byte1, byte2, byte3);
         }
     }

}


