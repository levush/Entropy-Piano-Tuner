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

#include "core/adapters/xmlfactory.h"
#include "core/system/eptexception.h"
#include "core/system/log.h"
#include "core/calculation/calculationmanager.h"

const QString PianoFileIOXml::FILE_TYPE_NAME("entropyPianoTunerFile");
const PianoFileIOXml::FileVersionType PianoFileIOXml::UNSET_FILE_VERSION(-1);
const PianoFileIOXml::FileVersionType PianoFileIOXml::CURRENT_FILE_VERSION(1);
const PianoFileIOXml::FileVersionType PianoFileIOXml::MIN_SUPPORTED_FILE_VERSION(1);

void PianoFileIOXml::write(QIODevice *device, const Piano &piano) const {
    EptAssert(device, "QIODevice may not be null");

    XmlWriterInterfacePtr writerPtr(XmlFactory::getDefaultWriter());
    XmlWriterInterface &writer(*writerPtr);

    writer.writeStartDocument();

    // root
    writer.writeStartElement(FILE_TYPE_NAME.toStdString());
    writer.writeAttribute("version", CURRENT_FILE_VERSION);

    // piano
    // ----------------------------------------------------------------------------------------
    writer.writeStartElement("piano");

    writer.writeAttribute("concertPitch", piano.getConcertPitch());
    writer.writeAttribute("type", piano.getPianoType());

    writer.writeTextElement("name", piano.getName());
    writer.writeTextElement("serialNumber", piano.getSerialNumber());
    writer.writeTextElement("manufactionYear", piano.getManufactionYear());
    writer.writeTextElement("manufactionLocation", piano.getManufactionLocation());
    writer.writeTextElement("tuningLocation", piano.getTuningLocation());
    writer.writeTextElement("tuningTimestamp", piano.getTuningTime());

    // keyboard
    writer.writeStartElement("keyboard");
    const Keyboard &keyboard = piano.getKeyboard();

    writer.writeAttribute("numberOfKeys", keyboard.getNumberOfKeys());
    writer.writeAttribute("keyNumberOfA", keyboard.getKeyNumberOfA4());
    writer.writeAttribute("numberOfBassKeys", keyboard.getNumberOfBassKeys());

    for (size_t keyIndex = 0; keyIndex < keyboard.size(); ++keyIndex) {
        const Key &key = keyboard[keyIndex];
        writer.writeStartElement("key");

        writer.writeAttribute("key", static_cast<int>(keyIndex));
        writer.writeAttribute("recordedFrequency", key.getRecordedFrequency());
        writer.writeAttribute("measuredInharmonicity", key.getMeasuredInharmonicity());
        writer.writeAttribute("computedFrequency", key.getComputedFrequency());
        writer.writeAttribute("tunedFrequency", key.getTunedFrequency());
        writer.writeAttribute("recorded", key.isRecorded());
        writer.writeAttribute("quality", key.getRecognitionQuality());

        // spectrum
        const Key::SpectrumType &spectrum(key.getSpectrum());
        writer.writeDoubleListElement("spectrum", spectrum);

        // peaks
        for (auto peak : key.getPeaks()) {
            writer.writeStartElement("peak");
            writer.writeAttribute("frequency", peak.first);
            writer.writeAttribute("intensity", peak.second);
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
        writer.writeAttribute("current", currentAlgorithm);
    }

    for (const SingleAlgorithmParametersPtr ad : parameters) {
        writer.writeStartElement("algorithm");

        writer.writeAttribute("name", ad->getAlgorithmName());

        auto doubleParameters = ad->getDoubleParameters();
        for (const auto &dp : doubleParameters) {
            writer.writeStartElement("parameter");

            writer.writeAttribute("type", "double");

            writer.writeAttribute("name", dp.first);
            writer.writeCharacters(dp.second);

            writer.writeEndElement();  // parameter
        }

        auto intParameters = ad->getIntParameters();
        for (const auto &ip : intParameters) {
            writer.writeStartElement("parameter");

            writer.writeAttribute("type", "int");

            writer.writeAttribute("name", ip.first);
            writer.writeCharacters(ip.second);

            writer.writeEndElement();  // parameter
        }

        auto stringParameters = ad->getStringParameters();
        for (const auto &sp : stringParameters) {
            writer.writeStartElement("parameter");

            writer.writeAttribute("type", "string");

            writer.writeAttribute("name", sp.first);
            writer.writeCharacters(sp.second);

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

    // write content as UTF-8
    std::wstring content = writer.close();
    QTextStream stream(device);
    stream.setCodec("UTF-8");
    stream << QString::fromStdWString(content);
    stream.flush();

    LogI("Successfully written xml to QIODevice");
}

void PianoFileIOXml::read(QIODevice *device, Piano &piano) {
    EptAssert(device, "QIODevice may not be null");

    auto startTime = std::chrono::high_resolution_clock::now();

    // preparation
    mFileVersion = UNSET_FILE_VERSION;

    // create reader
    XmlReaderInterfacePtr readerPtr = XmlFactory::getDefaultReader();
    XmlReaderInterface &reader(*readerPtr);

    // read all as UTF-8
    QTextStream stream(device);
    stream.setCodec("UTF-8");
    reader.openString(stream.readAll().toStdWString());

    while (!reader.atEnd()) {
        // read only start elements
        if (!reader.readNextStartElement()) {break;}

        if (reader.name() == FILE_TYPE_NAME.toStdString()) {
            // read header information

            mFileVersion = reader.queryIntAttribute("version", UNSET_FILE_VERSION);

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
            int type = reader.queryIntAttribute("type", piano.getPianoType());
            piano.setType(static_cast<piano::PianoType>(type));

            reader.queryRealAttributeRef("concertPitch", piano.getConcertPitch());

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
                    piano.setName(reader.queryWStringText());
                } else if (reader.name() == "serialNumber") {
                    piano.setSerialNumber(reader.queryWStringText());
                } else if (reader.name() == "manufactionYear") {
                    piano.setManufactureYear(reader.queryWStringText());
                } else if (reader.name() == "manufactionLocation") {
                    piano.setManufactureLocation(reader.queryWStringText());
                } else if (reader.name() == "tuningLocation") {
                    piano.setTuningLocation(reader.queryWStringText());
                } else if (reader.name() == "tuningTimestamp") {
                    std::wstring text = reader.queryWStringText();
                    if (text.size() > 0) {
                        piano.setTuningTime(text);
                    } else {
                        piano.setTuningTimeToCurrentTime();
                    }
                } else if (reader.name() == "keyboard") {
                    Keyboard &keyboard = piano.getKeyboard();
                    // read keyboard data
                    int numberOfKeys = reader.queryIntAttribute("numberOfKeys", keyboard.getNumberOfKeys());
                    int keyNumberOfA = reader.queryIntAttribute("keyNumberOfA", keyboard.getKeyNumberOfA4());
                    int numberOfBassKeys = reader.queryIntAttribute("numberOfBassKeys", keyboard.getNumberOfBassKeys());
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

                        int keyIndex = reader.queryIntAttribute("key", -1);
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
                        reader.queryRealAttributeRef("recordedFrequency", key.getRecordedFrequency());
                        reader.queryRealAttributeRef("measuredInharmonicity", key.getMeasuredInharmonicity());
                        reader.queryRealAttributeRef("computedFrequency", key.getComputedFrequency());
                        reader.queryRealAttributeRef("tunedFrequency", key.getTunedFrequency());
                        reader.queryBoolAttributeRef("recorded", key.isRecorded());
                        reader.queryRealAttributeRef("quality", key.getRecognitionQuality());

                        while (!reader.atEnd()) {
                            reader.readNext();

                            if (reader.isEndElement() && reader.name() == "key") {
                                break;
                            }

                            if (reader.isStartElement() == false) {continue;}

                            if (reader.name() == "spectrum") {
                                // spectrum
                                auto &spectrum = key.getSpectrum();

                                std::vector<double> readSpectrum = reader.queryDoubleVectorText();

                                if (spectrum.size() != readSpectrum.size()) {
                                    LogW("Spectrum sizes do not match");
                                }

                                spectrum = readSpectrum;
                            } else if (reader.name() == "peak") {
                                qreal frequency = reader.queryRealAttribute("frequency", -1);
                                qreal intensity = reader.queryRealAttribute("intensity", -1);
                                key.getPeaks()[frequency] = intensity;
                            } else {
                                LogW("Invalid child xml element '%s'", reader.name().c_str());
                            }
                        }
                    }
                } else if (reader.name() == "algorithms") {
                    if (reader.hasAttribute("current")) {
                        CalculationManager::getSingleton().setCurrentAlgorithmInformationById(reader.queryStringAttribute("current"));
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
                            std::string algorithmName = reader.queryStringAttribute("name");

                            SingleAlgorithmParametersPtr ad = piano.getAlgorithmParameters().getOrCreate(algorithmName);

                            // read a single algorithm
                            while (!reader.atEnd()) {
                                reader.readNext();

                                // break on algorithm end
                                if (reader.isEndElement() && reader.name() == "algorithm") {break;}

                                // read start elements only
                                if (reader.isStartElement() == false) {continue;}

                                if (reader.name() == "parameter") {
                                    std::string type = reader.queryStringAttribute("type");
                                    std::string id = reader.queryStringAttribute("name");

                                    if (type == "double") {
                                        ad->setDoubleParameter(id, reader.queryDoubleText());
                                    } else if (type == "int") {
                                        ad->setIntParameter(id, reader.queryIntText());
                                    } else if (type == "string") {
                                        ad->setStringParameter(id, reader.queryStringText());
                                    } else {
                                        LogW("Unknown parameter type '%s' with id '%s'", type.c_str(), id.c_str());
                                    }

                                } else {
                                    LogW("Unknown start element: %s", reader.name().c_str());
                                }

                            }
                        } else {
                            LogW("Unknown start element: %s", reader.name().c_str());
                        }
                    }
                } else {
                    LogW("Unknown start element: %s", reader.name().c_str());
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
