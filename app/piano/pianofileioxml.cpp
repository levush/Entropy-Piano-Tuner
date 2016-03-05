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

#include "pianofileioxml.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>
#include <chrono>
#include <sstream>

#include "core/system/eptexception.h"
#include "core/system/log.h"
#include "core/calculation/calculationmanager.h"

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
    // ----------------------------------------------------------------------------------------
    writer.writeStartElement("piano");

    writer.writeAttribute("concertPitch", QString::number(piano.getConcertPitch()));
    writer.writeAttribute("type", QString::number(piano.getPianoType()));

    writer.writeTextElement("name", QString::fromStdWString(piano.getName()));
    writer.writeTextElement("serialNumber", QString::fromStdWString(piano.getSerialNumber()));
    writer.writeTextElement("manufactionYear", QString::fromStdWString(piano.getManufactionYear()));
    writer.writeTextElement("manufactionLocation", QString::fromStdWString(piano.getManufactionLocation()));
    writer.writeTextElement("tuningLocation", QString::fromStdWString(piano.getTuningLocation()));
    writer.writeTextElement("tuningTimestamp", QString::fromStdWString(piano.getTuningTime()));

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


    // algorithm status
    // ----------------------------------------------------------------------------------------
    writer.writeStartElement("algorithms");

    const std::string &currentAlgorithm = CalculationManager::getSingleton().getCurrentAlgorithmId();
    const std::vector<SingleAlgorithmParametersPtr> &parameters = piano.getAlgorithmParameters().getParameters();

    if (currentAlgorithm.empty() == false) {
        writer.writeAttribute("current", QString::fromStdString(currentAlgorithm));
    }

    for (const SingleAlgorithmParametersPtr ad : parameters) {
        writer.writeStartElement("algorithm");

        writer.writeAttribute("name", QString::fromStdString(ad->getAlgorithmName()));

        auto doubleParameters = ad->getDoubleParameters();
        for (const auto &dp : doubleParameters) {
            writer.writeStartElement("parameter");

            writer.writeAttribute("type", "double");

            writer.writeAttribute("name", QString::fromStdString(dp.first));
            writer.writeCharacters(QString::number(dp.second));

            writer.writeEndElement();  // parameter
        }

        auto intParameters = ad->getIntParameters();
        for (const auto &ip : intParameters) {
            writer.writeStartElement("parameter");

            writer.writeAttribute("type", "int");

            writer.writeAttribute("name", QString::fromStdString(ip.first));
            writer.writeCharacters(QString::number(ip.second));

            writer.writeEndElement();  // parameter
        }

        auto stringParameters = ad->getStringParameters();
        for (const auto &sp : stringParameters) {
            writer.writeStartElement("parameter");

            writer.writeAttribute("type", "string");

            writer.writeAttribute("name", QString::fromStdString(sp.first));
            writer.writeCharacters(QString::fromStdString(sp.second));

            writer.writeEndElement();  // parameter
        }

        writer.writeEndElement();  // algorithm
    }


    writer.writeEndElement();  // algorithm

    writer.writeEndElement();  // piano
    // finished

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
        qreal value = reader.attributes().value(aname).toDouble(&ok);
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
        if (!reader.readNextStartElement()) {break;}

        if (reader.name() == FILE_TYPE_NAME) {
            // read header information

            mFileVersion = reader.attributes().value("version").toInt(&ok);

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
                    piano.setName(reader.readElementText().toStdWString());
                } else if (reader.name() == "serialNumber") {
                    piano.setSerialNumber(reader.readElementText().toStdWString());
                } else if (reader.name() == "manufactionYear") {
                    piano.setManufactureYear(reader.readElementText().toStdWString());
                } else if (reader.name() == "manufactionLocation") {
                    piano.setManufactureLocation(reader.readElementText().toStdWString());
                } else if (reader.name() == "tuningLocation") {
                    piano.setTuningLocation(reader.readElementText().toStdWString());
                } else if (reader.name() == "tuningTimestamp") {
                    QString text = reader.readElementText();
                    if (text.isEmpty()) {
                        piano.setTuningTime(text.toStdWString());
                    } else {
                        piano.setTuningTimeToCurrentTime();
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

                        // break if end element of keyboard element
                        if (reader.isEndElement() && reader.name() == "keyboard") {
                            break;
                        }

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

                            if (reader.isEndElement() && reader.name() == "key") {
                                break;
                            }

                            if (reader.isStartElement() == false) {continue;}

                            if (reader.name() == "spectrum") {
                                // spectrum
                                auto &spectrum = key.getSpectrum();
                                // pointer to the text
                                QString s = reader.readElementText();
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
                    }
                } else if (reader.name() == "algorithms") {
                    if (reader.attributes().hasAttribute("current")) {
                        CalculationManager::getSingleton().setCurrentAlgorithmInformationById(reader.attributes().value("current").toString().toStdString());
                    }

                    // read single algorithms
                    while (!reader.atEnd()) {
                        reader.readNext();

                        // break on algorithms end
                        if (reader.isEndElement() && reader.name() == "algorithms") {
                            break;
                        }

                        // read start elements only
                        if (reader.isStartElement() == false) {
                            continue;
                        }


                        if (reader.name() == "algorithm") {
                            // algorithm name
                            QString algorithmName = reader.attributes().value("name").toString();

                            SingleAlgorithmParametersPtr ad = piano.getAlgorithmParameters().getOrCreate(algorithmName.toStdString());

                            // read a single algorithm
                            while (!reader.atEnd()) {
                                reader.readNext();

                                // break on algorithm end
                                if (reader.isEndElement() && reader.name() == "algorithm") {break;}

                                // read start elements only
                                if (reader.isStartElement() == false) {continue;}

                                if (reader.name() == "parameter") {
                                    QStringRef type = reader.attributes().value("type");
                                    std::string id = reader.attributes().value("name").toString().toStdString();
                                    QString value = reader.readElementText();

                                    if (type == "double") {
                                        ad->setDoubleParameter(id, value.toDouble());
                                    } else if (type == "int") {
                                        ad->setIntParameter(id, value.toInt());
                                    } else if (type == "string") {
                                        ad->setStringParameter(id, value.toStdString());
                                    } else {
                                        LogW("Unknown parameter type '%s' with id '%s' and value '%s'", type.toString().toStdString().c_str(), id.c_str(), value.toStdString().c_str());
                                    }

                                } else {
                                    LogW("Unknown start element: %s", reader.name().toString().toStdString().c_str());
                                }

                            }
                        } else {
                            LogW("Unknown start element: %s", reader.name().toString().toStdString().c_str());
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
