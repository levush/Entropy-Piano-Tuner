#include "qtpcmdevice.h"
#include "audioplayerthreadforqt.h"

void QtPCMDevice::setWriter(PCMWriterInterface *writer) {
    mWriter = writer;
}

qint64 QtPCMDevice::readData(char *data, qint64 maxSize) {
    maxSize = maxSize * 0.5;

    // Determine the scaling constant
    const auto scaling = std::numeric_limits<AudioPlayerThreadForQt::DataFormat>::max();

    qint64 outSize = 0;
    if (mWriter) {
        AudioBase::PacketType packet(maxSize / sizeof(AudioPlayerThreadForQt::DataFormat));
        if (mWriter->generateAudioSignal(packet)) {
            AudioPlayerThreadForQt::DataFormat *p = reinterpret_cast<AudioPlayerThreadForQt::DataFormat*>(data);
            for (qint64 i = 0; i < packet.size(); ++i) {
                *p = scaling * packet[i];
                ++p;
            }
            outSize = packet.size() * sizeof(AudioPlayerThreadForQt::DataFormat);
        }
    }

    if (outSize == 0) {
        std::fill(data, data + maxSize, 0);
        return maxSize;
    }

    return outSize;
}

qint64 QtPCMDevice::writeData(const char *data, qint64 maxSize) {
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    return 0;
}
