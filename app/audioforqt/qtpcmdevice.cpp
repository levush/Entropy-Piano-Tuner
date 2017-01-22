#include "qtpcmdevice.h"
#include "audioplayerforqt.h"

QtPCMDevice::QtPCMDevice(AudioInterfaceForQt *audioInterface)
    : mAudioInterface(audioInterface) {
    EptAssert(audioInterface, "A valid audio interface is required");
}

bool QtPCMDevice::open(OpenMode mode) {
    if (mDevice) {
        mDevice->open(mAudioInterface);
    }
    return QIODevice::open(mode);
}

void QtPCMDevice::close() {
    if (mDevice) {
        mDevice->close();
    }

    QIODevice::close();
}

void QtPCMDevice::setDevice(PCMDevice *device) {
    if (isOpen() && mDevice) {
        mDevice->close();
    }
    mDevice = device;
    if (isOpen() && mDevice) {
        mDevice->open(mAudioInterface);
    }
}

qint64 QtPCMDevice::readData(char *data, qint64 maxSize) {
    if (mDevice) {
        return mDevice->read(data, maxSize);
    }

    std::fill(data, data + maxSize, 0);
    return maxSize;
}

qint64 QtPCMDevice::writeData(const char *data, qint64 maxSize) {
    if (mDevice) {
        return mDevice->write(data, maxSize);
    }

    return 0;
}
