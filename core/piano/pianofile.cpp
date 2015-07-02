/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
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

#include "pianofile.h"

#include <iomanip>
#include <ctime>
#include <sstream>
#include <chrono>
#include <fstream>
#include <cmath>

#include "../system/eptexception.h"
#include "../adapters/filemanager.h"
#include "../system/log.h"
#include "../thirdparty/timesupport/timesupport.h"
#include "piano.h"

const std::string PianoFile::FILE_TYPE_NAME("EntropyPianoTunerFile");
const PianoFile::FileVersionType PianoFile::UNSET_FILE_VERSION(-1);
const PianoFile::FileVersionType PianoFile::CURRENT_FILE_VERSION(1);
const PianoFile::FileVersionType PianoFile::MIN_SUPPORTED_FILE_VERSION(1);

PianoFile::PianoFile()
    : mFileVersion(-1) {
}

PianoFile::~PianoFile() {
}

bool PianoFile::read(const std::string &absolutePath, Piano &piano) {
    // change locale to be sure to read doubles in classic format
    std::locale oldLocale(std::locale::global(std::locale::classic()));
    try {
        readXmlFile(absolutePath, piano);
    } catch (...) {
        std::locale::global(oldLocale);
        throw;
    }

    std::locale::global(oldLocale);

    return true;
}

bool PianoFile::write(const std::string &absolutePath, const Piano &piano, piano::FileType fileType) const {
    // change locale to be sure to write doubles in classic format
    std::locale oldLocale(std::locale::global(std::locale::classic()));
    try {
        if (fileType == piano::FT_EPT) {
            writeXmlFile(absolutePath, piano);
        } else if (fileType == piano::FT_CSV) {
            writeCsvFile(absolutePath, piano);
        } else {
            EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Writing of file type not supported yet.");
        }
    } catch (...) {
        std::locale::global(oldLocale);
        throw;
    }

    std::locale::global(oldLocale);

    return true;
}

void PianoFile::readXmlFile(const std::string &absolutePath, Piano &piano) {
    auto startTime = std::chrono::high_resolution_clock::now();

    tinyxml2::XMLDocument doc(true, tinyxml2::COLLAPSE_WHITESPACE);
    doc.LoadFile(absolutePath.c_str());
    if (doc.Error()) {
        std::stringstream errormsg;
        errormsg << "Error " << doc.ErrorID() << ": " << doc.ErrorName() << " - File: " << absolutePath;
        EPT_EXCEPT(EptException::ERR_CANNOT_READ_FROM_FILE, errormsg.str());
    }

    auto parsedTime = std::chrono::high_resolution_clock::now();

    int parsedDuration = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(parsedTime - startTime).count());
    LogI("Succesfully loaded Xml-File: %s. This took %i ms", absolutePath.c_str(), parsedDuration);

    tinyxml2::XMLElement *root = doc.FirstChildElement();
    if (!root) {
        LogW("Xml-document has no root node");
        return;
    }

    // read version
    mFileVersion = UNSET_FILE_VERSION;
    root->QueryIntAttribute("version", &mFileVersion);

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

    // parse all child elements
    for (tinyxml2::XMLElement *child = root->FirstChildElement(); child;
         child = child->NextSiblingElement()) {
        if (strcmp(child->Value(), "piano") == 0) {
            read(child, piano);
        }
    }

    auto contentParseTime = std::chrono::high_resolution_clock::now();

    int contentTime = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(contentParseTime - parsedTime).count());
    int totalTime = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(contentParseTime - startTime).count());

    LogI("Content parsed in %i ms. Complete file opening took %i ms.", contentTime, totalTime);

}

void PianoFile::writeXmlFile(const std::string &absolutePath, const Piano &piano) const {
    tinyxml2::XMLDocument doc;

    // create xml declaration header
    doc.InsertEndChild(doc.NewDeclaration());

    tinyxml2::XMLElement *root = doc.NewElement("entropyPianoTunerFile");
    doc.InsertEndChild(root);
    root->SetAttribute("version", CURRENT_FILE_VERSION);

    // piano
    tinyxml2::XMLElement *pianoElement = doc.NewElement("piano");
    root->InsertEndChild(pianoElement);
    write(pianoElement, piano);

    // save the file
    doc.SaveFile(absolutePath.c_str());
    if (doc.Error()) {
        std::stringstream errormsg;
        errormsg << "Error " << doc.ErrorID() << ": " << doc.ErrorName() << " - File: " << absolutePath;
        EPT_EXCEPT(EptException::ERR_CANNOT_WRITE_TO_FILE, errormsg.str());
    }

    LogI("Succesfully saved Xml-File: %s", absolutePath.c_str());
}

std::string PianoFile::getText(const tinyxml2::XMLElement *element) {
    if (!element->GetText()) {
        return std::string();
    } else {
        return std::string(element->GetText());
    }
}

void PianoFile::createTextXMLElement(tinyxml2::XMLElement *parent, const char *label, const char *text) const {
    tinyxml2::XMLElement *e = parent->GetDocument()->NewElement(label);
    parent->InsertEndChild(e);
    e->SetText(text);
}

void PianoFile::read(const tinyxml2::XMLElement *e, Piano &piano) {
    EptAssert(e, "XMLElement has to exist.");
    int type = piano.getPianoType();

    e->QueryDoubleAttribute("concertPitch", &piano.getConcertPitch());
    e->QueryIntAttribute("type", &type);
    piano.setType(static_cast<piano::PianoType>(type));

    for (const tinyxml2::XMLElement *data = e->FirstChildElement(); data;
         data = data->NextSiblingElement()) {
        if (strcmp(data->Value(), "name") == 0) {
            piano.setName(getText(data));
        } else if (strcmp(data->Value(), "serialNumber") == 0) {
            piano.setSerialNumber(getText(data));
        } else if (strcmp(data->Value(), "manufactionYear") == 0) {
            piano.setManufactureYear(getText(data));
        } else if (strcmp(data->Value(), "manufactionLocation") == 0) {
            piano.setManufactureLocation(getText(data));
        } else if (strcmp(data->Value(), "tuningLocation") == 0) {
            piano.setTuningLocation(getText(data));
        } else if (strcmp(data->Value(), "tuningTimestamp") == 0) {
            if (data->GetText()) {
                piano.setTuningTime(data->GetText());
            } else {
                piano.setNow();
            }
        } else if (strcmp(data->Value(), "keyboard") == 0) {
            read(data, piano.getKeyboard());
        }
    }
}

void PianoFile::write(tinyxml2::XMLElement *e, const Piano &piano) const {
    e->SetAttribute("concertPitch", piano.getConcertPitch());
    e->SetAttribute("type", piano.getPianoType());

    createTextXMLElement(e, "name", piano.getName().c_str());
    createTextXMLElement(e, "serialNumber", piano.getSerialNumber().c_str());
    createTextXMLElement(e, "manufactionYear", piano.getManufactionYear().c_str());
    createTextXMLElement(e, "manufactionLocation", piano.getManufactionLocation().c_str());
    createTextXMLElement(e, "tuningLocation", piano.getTuningLocation().c_str());
    createTextXMLElement(e, "tuningTimestamp", piano.getTuningTime().c_str());

    // write key data
    tinyxml2::XMLElement *keyboardElem = e->GetDocument()->NewElement("keyboard");
    e->InsertEndChild(keyboardElem);
    write(keyboardElem, piano.getKeyboard());

}

void PianoFile::read(const tinyxml2::XMLElement *e, Keyboard &keyboard) {
    int numberOfKeys = keyboard.getNumberOfKeys();
    int keyNumberOfA = keyboard.getKeyNumberOfA4();
    e->QueryIntAttribute("numberOfKeys", &numberOfKeys);
    e->QueryIntAttribute("keyNumberOfA", &keyNumberOfA);
    keyboard.setNumberOfKeys(numberOfKeys, keyNumberOfA);

    for (const tinyxml2::XMLElement *keyElement = e->FirstChildElement(); keyElement;
         keyElement = keyElement->NextSiblingElement()) {
        if (strcmp(keyElement->Value(), "key") == 0) {
            int key = -1;
            keyElement->QueryAttribute("key", &key);
            if (key < 0) {
                LogE("Key attribute not provided.");
                return;
            }

            // clear the peaks list
            keyboard[key].getPeaks().clear();

            // reset the specturum
            std::fill(keyboard[key].getSpectrum().begin(), keyboard[key].getSpectrum().end(), 0);

            // read data
            keyElement->QueryAttribute("recordedFrequency", &keyboard[key].getRecordedFrequency());
            keyElement->QueryAttribute("measuredInharmonicity", &keyboard[key].getMeasuredInharmonicity());
            keyElement->QueryAttribute("computedFrequency", &keyboard[key].getComputedFrequency());
            keyElement->QueryAttribute("tunedFrequency", &keyboard[key].getTunedFrequency());
            keyElement->QueryAttribute("recorded", &keyboard[key].isRecorded());
            keyElement->QueryAttribute("quality", &keyboard[key].getRecognitionQuality());

            for (const tinyxml2::XMLElement *child = keyElement->FirstChildElement(); child;
                 child = child->NextSiblingElement()) {
                if (strcmp(child->Value(), "spectrum") == 0) {
                    // spectrum
                    auto &spectrum = keyboard[key].getSpectrum();
                    // pointer to the text
                    const char *s = child->GetText();
                    // pointer to the last char that was read by strtod
                    char *p = 0;
                    // pointer to the data structure to fill
                    double *spec = spectrum.data();
                    do {
                        // read value
                        double val = strtod(s, &p);
                        // nothing to read, finish
                        if (s == p) break;

                        // store value and increase write position
                        *spec = (val);
                        ++spec;

                        // check if we reached the end
                        if (spec == &spectrum[0] + Key::NumberOfBins) {break;}

                        // adjust read position
                        s = p+1;
                    } while (*p);
                } else if (strcmp(child->Value(), "peak") == 0) {
                    // peak
                    double frequency = -1;
                    double intensity = -1;
                    child->QueryDoubleAttribute("frequency", &frequency);
                    child->QueryDoubleAttribute("intensity", &intensity);
                    keyboard[key].getPeaks()[frequency] = intensity;
                }
            }
        }
    }
}

void PianoFile::write(tinyxml2::XMLElement *e, const Keyboard &keyboard) const {
    assert(e);
    e->SetAttribute("numberOfKeys", keyboard.getNumberOfKeys());
    e->SetAttribute("keyNumberOfA", keyboard.getKeyNumberOfA4());

    for (size_t key = 0; key < keyboard.size(); ++key) {
        tinyxml2::XMLElement *keyElement = e->GetDocument()->NewElement("key");
        e->InsertEndChild(keyElement);

        keyElement->SetAttribute("key", static_cast<int>(key));

        keyElement->SetAttribute("recordedFrequency", keyboard[key].getRecordedFrequency());
        keyElement->SetAttribute("measuredInharmonicity", keyboard[key].getMeasuredInharmonicity());
        keyElement->SetAttribute("computedFrequency", keyboard[key].getComputedFrequency());
        keyElement->SetAttribute("tunedFrequency", keyboard[key].getTunedFrequency());
        keyElement->SetAttribute("recorded", keyboard[key].isRecorded());
        keyElement->SetAttribute("quality", keyboard[key].getRecognitionQuality());

        // spectrum
        const Key::SpectrumType &spectrum(keyboard[key].getSpectrum());
        std::stringstream oss;
        for (size_t spec = 0; spec < spectrum.size() - 1; ++spec) {
            // all but last with space
            oss << spectrum[spec] << " ";
        }
        // last without space
        oss << spectrum[spectrum.size() - 1];
        tinyxml2::XMLElement *specElement = e->GetDocument()->NewElement("spectrum");
        keyElement->InsertEndChild(specElement);
        specElement->SetText(oss.str().c_str());

        // peaks
        for (auto peak : keyboard[key].getPeaks()) {
            tinyxml2::XMLElement *peakElement = e->GetDocument()->NewElement("peak");
            keyElement->InsertEndChild(peakElement);
            peakElement->SetAttribute("frequency", peak.first);
            peakElement->SetAttribute("intensity", peak.second);
        }
    }
}


// ==================================================================================================
// CSV


void PianoFile::writeCsvFile(const std::string &absolutePath, const Piano &piano) const {
    std::ofstream stream;
    stream.open(absolutePath);
    if (!stream.is_open()) {
        EPT_EXCEPT(EptException::ERR_CANNOT_WRITE_TO_FILE, "Cannot write to file '" + absolutePath + "'");
    }

    // only write
    // key index + 1, inharmonicity, recorded frequency, computed frequency, tuned frequency, quality

    // header
    stream << "\"Key index\",\"Inharmonicity\",\"Recorded frequency\",\"Recorded deviation\",\"Computed frequency\",\"Computed deviation\",\"Tuned frequency\",\"Tuned deviation\",\"Quality\"" << std::endl;

    // data
    const int A4 = piano.getKeyboard().getKeyNumberOfA4();
    const Key &Akey = piano.getKey(A4);
    auto cents = [A4] (int i, double f, double fA4)
    {
        double ET = pow(2.0,(i-A4)/12.0);
        if (fA4 == 0) return 0.0;
        else return 1200.0 * std::log(f/fA4/ET)/std::log(2.0);
    };
    for (int i = 0; i < piano.getKeyboard().getNumberOfKeys(); ++i) {
        const Key &key = piano.getKeyboard()[i];
        stream << std::setw(3) << std::left << i + 1 << std::fixed << std::right
               << ", " << std::setw(8) << std::setprecision(5) << key.getMeasuredInharmonicity()
               << ", " << std::setw(8) << std::setprecision(2) << key.getRecordedFrequency()
               << ", " << std::setw(6) << std::setprecision(1) << cents(i,key.getRecordedFrequency(),Akey.getRecordedFrequency())
               << ", " << std::setw(8) << std::setprecision(2) << key.getComputedFrequency()
               << ", " << std::setw(6) << std::setprecision(1) << cents(i,key.getComputedFrequency(),Akey.getComputedFrequency())
               << ", " << std::setw(8) << std::setprecision(2) << key.getTunedFrequency()
               << ", " << std::setw(6) << std::setprecision(1) << cents(i,key.getTunedFrequency(),piano.getConcertPitch())
               << ", " << std::setw(6) << std::setprecision(2) << key.getRecognitionQuality()
               << std::endl;
    }
}
