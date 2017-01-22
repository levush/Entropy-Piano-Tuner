#include "pcmdevice.h"
#include "audiointerface.h"
#include "core/system/eptexception.h"
#include "core/system/log.h"

void PCMDevice::open(AudioInterface *audioInterface)
{
    EptAssert(audioInterface, "Audio interface is nullptr");

    mAudioInterface = audioInterface;
}

void PCMDevice::close()
{
    mAudioInterface = nullptr;
}

int PCMDevice::getSampleRate() const {
    if (mAudioInterface) {
        return mAudioInterface->getSamplingRate();
    }

    LogE("Requested sampling rate though device is not open.");
    return 0;
}

int PCMDevice::getChannels() const {
    if (mAudioInterface) {
        return mAudioInterface->getChannelCount();
    }

    LogE("Requested channel count though device is not open.");

    return 1;
}
