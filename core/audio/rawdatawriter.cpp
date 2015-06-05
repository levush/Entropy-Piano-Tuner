#include "rawdatawriter.h"

RawDataWriter::RawDataWriter(size_t packetSize)
    : mPacket(packetSize)
{

}

AudioBase::PacketType RawDataWriter::readPacket(size_t size) {
    std::lock_guard<std::mutex> lock(mPacketMutex);
    return mPacket.readData(size);
}

void RawDataWriter::clearPacket() {
    std::lock_guard<std::mutex> lock(mPacketMutex);
    mPacket.clear();
}

size_t RawDataWriter::getFreePacketSize() const {
    std::lock_guard<std::mutex> lock(mPacketMutex);
    return mPacket.maximum_size() - mPacket.size();
}

void RawDataWriter::writeData(AudioBase::PacketType d) {
    std::lock_guard<std::mutex> lock(mPacketMutex);
    for (auto &pcm : d) {mPacket.push_back(pcm);}
}
