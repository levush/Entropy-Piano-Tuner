#include "qtpcmdevice.h"
#include "audioplayerforqt.h"

void QtPCMDevice::setWriter(PCMWriterInterface *writer) {
    mWriter = writer;
}

qint64 QtPCMDevice::readData(char *data, qint64 maxSize) {
    maxSize = maxSize * 0.5;

    // Determine the scaling constant
    const auto scaling = std::numeric_limits<AudioPlayerForQt::DataFormat>::max();

    qint64 outSize = 0;
    if (mWriter) {
        AudioBase::PacketType packet(maxSize / sizeof(AudioPlayerForQt::DataFormat));
        if (mWriter->generateAudioSignal(packet)) {
            AudioPlayerForQt::DataFormat *p = reinterpret_cast<AudioPlayerForQt::DataFormat*>(data);
            for (size_t i = 0; i < packet.size(); ++i) {
                *p = scaling * packet[i];
                ++p;
            }
            outSize = packet.size() * sizeof(AudioPlayerForQt::DataFormat);
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
