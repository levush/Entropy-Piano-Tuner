#include "pianofileioxml.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QTextStream>
#include <chrono>
#include <sstream>

#include "core/system/eptexception.h"
#include "core/system/log.h"

const QString PianoFileIOXml::FILE_TYPE_NAME("entropyPianoTunerFile");
const PianoFileIOXml::FileVersionType PianoFileIOXml::UNSET_FILE_VERSION(-1);
const PianoFileIOXml::FileVersionType PianoFileIOXml::CURRENT_FILE_VERSION(1);
const PianoFileIOXml::FileVersionType PianoFileIOXml::MIN_SUPPORTED_FILE_VERSION(1);

void PianoFileIOXml::write(QIODevice *device, const Piano &piano) const {
    EptAssert(device, "QIODevice may not be null");

    QXmlStreamWriter writer(device);

    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    // root
    writer.writeStartElement(FILE_TYPE_NAME);
    writer.writeAttribute("version", QString::number(CURRENT_FILE_VERSION));

    // piano
    writer.writeStartElement("piano");

    writer.writeAttribute("concertPitch", QString::number(piano.getConcertPitch()));
    writer.writeAttribute("type", QString::number(piano.getPianoType()));

    writer.writeTextElement("name", QString::fromStdString(piano.getName()));
    writer.writeTextElement("serialNumber", QString::fromStdString(piano.getSerialNumber()));
    writer.writeTextElement("manufactionYear", QString::fromStdString(piano.getManufactionYear()));
    writer.writeTextElement("manufactionLocation", QString::fromStdString(piano.getManufactionLocation()));
    writer.writeTextElement("tuningLocation", QString::fromStdString(piano.getTuningLocation()));
    writer.writeTextElement("tuningTimestamp", QString::fromStdString(piano.getTuningTime()));

    // keyboard
    writer.writeStartElement("keyboard");
    const Keyboard &keyboard = piano.getKeyboard();

    writer.writeAttribute("numberOfKeys", QString::number(keyboard.getNumberOfKeys()));
    writer.writeAttribute("keyNumberOfA", QString::number(keyboard.getKeyNumberOfA4()));
    writer.writeAttribute("numberOfBassKeys", QString::number(keyboard.getNumberOfBassKeys()));

    for (size_t keyIndex = 0; keyIndex < keyboard.size(); ++keyIndex) {
        const Key &key = keyboard[keyIndex];
        writer.writeStartElement("key");

        writer.writeAttribute("key", QString::number(keyIndex));
        writer.writeAttribute("recordedFrequency", QString::number(key.getRecordedFrequency()));
        writer.writeAttribute("measuredInharmonicity", QString::number(key.getMeasuredInharmonicity()));
        writer.writeAttribute("computedFrequency", QString::number(key.getComputedFrequency()));
        writer.writeAttribute("tunedFrequency", QString::number(key.getTunedFrequency()));
        writer.writeAttribute("recorded", key.isRecorded() ? "1" : "0");
        writer.writeAttribute("quality", QString::number(key.getRecognitionQuality()));

        // spectrum
        const Key::SpectrumType &spectrum(key.getSpectrum());
        QString tsstring;
        QTextStream ts(&tsstring);
        ts.setCodec(writer.codec());
        ts.setLocale(QLocale(QLocale::English));
        for (size_t spec = 0; spec < spectrum.size() - 1; ++spec) {
            // all but last with space
            ts << spectrum[spec] << " ";
        }
        // last without space
        ts << spectrum[spectrum.size() - 1];

        writer.writeTextElement("spectrum", tsstring);

        // peaks
        for (auto peak : key.getPeaks()) {
            writer.writeStartElement("peak");
            writer.writeAttribute("frequency", QString::number(peak.first));
            writer.writeAttribute("intensity", QString::number(peak.second));
            writer.writeEndElement();  // peak
        }

        writer.writeEndElement();  // key
    }

    writer.writeEndElement();  // keyboard

    // finish

    writer.writeEndElement();  // piano

    writer.writeEndElement();  // entropyPianoTunerFile

    writer.writeEndDocument();

    if (writer.hasError()) {
        EPT_EXCEPT(EptException::ERR_CANNOT_WRITE_TO_FILE, "An error occured when writing to an xml stream.")
    }

    LogI("Successfully written xml to QIODevice");
}

void PianoFileIOXml::read(QIODevice *device, Piano &piano) {
    EptAssert(device, "QIODevice may not be null");

    auto startTime = std::chrono::high_resolution_clock::now();

    // preparation
    mFileVersion = UNSET_FILE_VERSION;

    // read
    QXmlStreamReader reader(device);

    bool ok = true;
    auto CheckAttributeParseError = [&reader, &ok](QString aname) {
        if (!ok) {LogW("Could not parse attribute '%s' with value '%s'", aname.toStdString().c_str(), reader.attributes().value(aname).toString().toStdString().c_str());}
    };
    auto ReadIntAttribute = [&](QString aname, int defaultValue) {
        if (!reader.attributes().hasAttribute(aname)) {
            return defaultValue;
        }
        int value = reader.attributes().value(aname).toInt(&ok);
        CheckAttributeParseError(aname);
        if (!ok) {
            LogI("Using default value");
            return defaultValue;
        }
        return value;
    };
    auto ReadRealAttribute = [&](QString aname, qreal defaultValue) {
        if (!reader.attributes().hasAttribute(aname)) {
            return defaultValue;
        }
        qreal value = reader.attributes().value(aname).toInt(&ok);
        CheckAttributeParseError(aname);
        if (!ok) {
            LogI("Using default value");
            return defaultValue;
        }
        return value;
    };
    auto ReadRealAttributeRef = [&](QString aname, qreal *valueRef) {
        *valueRef = ReadRealAttribute(aname, *valueRef);
    };
    auto ReadBoolAttribute = [&](QString aname, bool defaultValue) {
        return ReadIntAttribute(aname, defaultValue ? 1 : 0) ? true : false;
    };
    auto ReadBoolAttributeRef = [&](QString aname, bool *valueRef) {
        *valueRef = ReadBoolAttribute(aname, *valueRef);
    };



    while (!reader.atEnd()) {
        // read only start elements
        reader.readNextStartElement();

        if (reader.name() == FILE_TYPE_NAME) {
            // read header information

            mFileVersion = reader.attributes().value("version").toInt(&ok);
            CheckAttributeParseError("version");

            if (mFileVersion == UNSET_FILE_VERSION) {
                LogW("No file version specified. Trying to continue with minimum supported.");
                mFileVersion = MIN_SUPPORTED_FILE_VERSION;
            }
            // check version
            if (mFileVersion < MIN_SUPPORTED_FILE_VERSION) {
                std::stringstream errormsg;
                errormsg << "File version " << mFileVersion << " is not supported anymore. Minimum file version is " << MIN_SUPPORTED_FILE_VERSION;
                EPT_EXCEPT(EptException::ERR_CANNOT_READ_FROM_FILE, errormsg.str());
            }
        }
        else if (reader.name() == "piano") {
            // read piano
            int type = ReadIntAttribute("type", piano.getPianoType());
            piano.setType(static_cast<piano::PianoType>(type));

            qreal concertPitch = ReadRealAttribute("concertPitch", piano.getConcertPitch());
            piano.setConcertPitch(concertPitch);

            while (!reader.atEnd()) {
                reader.readNext();

                // break on piano end
                if (reader.isEndElement() && reader.name() == "piano") {
                    break;
                }

                // read start elements only
                if (reader.isStartElement() == false) {
                    continue;
                }

                if (reader.name() == "name") {
                    piano.setName(reader.text().toString().toStdString());
                } else if (reader.name() == "serialNumber") {
                    piano.setSerialNumber(reader.text().toString().toStdString());
                } else if (reader.name() == "manufactionYear") {
                    piano.setManufactureYear(reader.text().toString().toStdString());
                } else if (reader.name() == "manufactionLocation") {
                    piano.setManufactureLocation(reader.text().toString().toStdString());
                } else if (reader.name() == "tuningLocation") {
                    piano.setTuningLocation(reader.text().toString().toStdString());
                } else if (reader.name() == "tuningTimestamp") {
                    QString text = reader.text().toString();
                    if (text.isEmpty()) {
                        piano.setTuningTime(text.toStdString());
                    } else {
                        piano.setTuningTimeToActualTime();
                    }
                } else if (reader.name() == "keyboard") {
                    Keyboard &keyboard = piano.getKeyboard();
                    // read keyboard data
                    int numberOfKeys = ReadIntAttribute("numberOfKeys", keyboard.getNumberOfKeys());
                    int keyNumberOfA = ReadIntAttribute("keyNumberOfA", keyboard.getKeyNumberOfA4());
                    int numberOfBassKeys = ReadIntAttribute("numberOfBassKeys", keyboard.getNumberOfBassKeys());
                    keyboard.changeKeyboardConfiguration(numberOfKeys, keyNumberOfA);
                    keyboard.setNumberOfBassKeys(numberOfBassKeys);

                    while (!reader.atEnd()) {
                        reader.readNext();

                        // read only start elements
                        if (!reader.isStartElement()) {
                            continue;
                        }

                        int keyIndex = ReadIntAttribute("key", -1);
                        if (keyIndex < -1 || keyIndex > numberOfKeys - 1) {
                            LogW("Invalid key index of %i", keyIndex);
                            continue;
                        }

                        Key &key = keyboard[keyIndex];

                        // clear the peaks list
                        key.getPeaks().clear();

                        // reset the specturum
                        std::fill(key.getSpectrum().begin(), key.getSpectrum().end(), 0);

                        // read data
                        ReadRealAttributeRef("recordedFrequency", &key.getRecordedFrequency());
                        ReadRealAttributeRef("measuredInharmonicity", &key.getMeasuredInharmonicity());
                        ReadRealAttributeRef("computedFrequency", &key.getComputedFrequency());
                        ReadRealAttributeRef("tunedFrequency", &key.getTunedFrequency());
                        ReadBoolAttributeRef("recorded", &key.isRecorded());
                        ReadRealAttributeRef("quality", &key.getRecognitionQuality());

                        while (!reader.atEnd()) {
                            reader.readNext();

                            if (reader.isEndElement() && reader.name() == "keyboard") {
                                break;
                            }

                            if (reader.isStartElement() == false) {continue;}

                            if (reader.name() == "spectrum") {
                                // spectrum
                                auto &spectrum = key.getSpectrum();
                                // pointer to the text
                                QString s = reader.text().toString();
                                QTextStream ts(&s, QIODevice::ReadOnly);
                                ts.setLocale(QLocale(QLocale::English));

                                for (size_t i = 0; i < spectrum.size(); ++i) {
                                    ts >> spectrum[i];
                                }
                            } else if (reader.name() == "peak") {
                                qreal frequency = ReadRealAttribute("frequency", -1);
                                qreal intensity = ReadRealAttribute("intensity", -1);
                                key.getPeaks()[frequency] = intensity;
                            } else {
                                LogW("Invalid child xml element '%s'", reader.name().toString().toStdString().c_str());
                            }
                        }

                        // break if end element of keyboard element
                        if (reader.isEndElement() && reader.name() == "keyboard") {
                            break;
                        }
                    }
                } else {
                    LogW("Unknown start element: %s", reader.name().toString().toStdString().c_str());
                }
            }
        }
    }

    if (reader.hasError()) {
        EPT_EXCEPT(EptException::ERR_CANNOT_WRITE_TO_FILE, "An error occured when writing to an xml stream.")
    }

    // timing
    auto contentParseTime = std::chrono::high_resolution_clock::now();
    int contentTime = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(contentParseTime - startTime).count());

    LogI("XML file read. Content parsed in %i ms.", contentTime);
}
