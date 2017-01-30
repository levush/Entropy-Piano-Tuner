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

#include "algorithminformationparser.h"

#include <limits>
#include <locale>

#include "../system/eptexception.h"
#include "../system/log.h"
#include "../settings.h"
#include "../adapters/filemanager.h"

std::shared_ptr<const AlgorithmInformation> AlgorithmInformationParser::parse(const AlgorithmFactoryDescription &algorithmFactory) const {
    const std::string &algorithmId(algorithmFactory.getAlgorithmName());
    const std::string &algorithmVersion(algorithmFactory.getVersion());

    XmlReaderInterfacePtr readerPtr = XmlFactory::getDefaultReader();
    XmlReaderInterface &reader = *readerPtr;

    std::wstring fileContent(FileManager::getSingleton().getAlgorithmInformationFileContent(algorithmId));

    reader.openString(fileContent);

    std::wstring author, name, year, description;
    AlgorithmInformation::ParameterListType params;

    while (!reader.atEnd()) {
        reader.readNext();

        if (!reader.isStartElement()) {continue;}

        if (reader.name() == "algorithm") {
            // root node content
            year = reader.queryWStringAttribute("year");
            author = reader.queryWStringAttribute("author");
        } else if (reader.name() == "name") {
            name = parseLanguageString(reader);
        } else if (reader.name() == "description") {
            description = parseLanguageString(reader);
        } else if (reader.name() == "param") {
            params.push_back(parseAlgorithmParameter(reader));
        }
    }

    return std::shared_ptr<const AlgorithmInformation>(
                new AlgorithmInformation(algorithmId, algorithmVersion, name, description, year, author, params));
}

std::wstring AlgorithmInformationParser::parseLanguageString(XmlReaderInterface &reader) const {
    std::string languageId = Settings::getSingleton().getUserLanguageId();

    std::wstring defaultLanguageString;

    const std::string currentNodeName = reader.name();
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isEndElement() && reader.name() == currentNodeName) {break;}
        if (reader.isStartElement() == false) {continue;}

        if (reader.name() != "string") {
            EPT_EXCEPT(EptException::ERR_INVALIDPARAMS, "Only string names are expected when parsing elements of type '"
                       + currentNodeName + "', but element with name '"
                       + reader.name() + "' was given.");
        }

        if (reader.hasAttribute("lang")) {
            if (languageId == reader.queryStringAttribute("lang")) {
                return reader.queryWStringText();
            }
        } else {
            // default
            defaultLanguageString = reader.queryWStringText();

            if (languageId == "en") {
                // default language is en
                return defaultLanguageString;
            }
        }

        // not the desired string
    }

    // language string not found. Check if default string exist, then return this one
    if (defaultLanguageString.size() > 0) {
        LogD("Element '%s' has missing translation for '%s'", currentNodeName.c_str(), languageId.c_str());
        return defaultLanguageString;
    }

    EPT_EXCEPT(EptException::ERR_INVALIDPARAMS, "Element '" + currentNodeName + "' has no child elements with name 'string'");
}

AlgorithmParameterDescription AlgorithmInformationParser::parseAlgorithmParameter(XmlReaderInterface &reader) const {

    const std::string id = reader.queryStringAttribute("id");
    const std::string type = reader.queryStringAttribute("type");

    std::wstring label, description;

    AlgorithmParameterDescription parameter;
    AlgorithmParameterDescription::StringParameterList &list = parameter.getStringList();

    parameter.getID() = id;

    // nonspecific parameters
    reader.queryBoolAttributeRef("lineEdit", parameter.displayLineEdit());
    reader.queryBoolAttributeRef("spinBox", parameter.displaySpinBox());
    reader.queryBoolAttributeRef("slider", parameter.displaySlider());
    reader.queryBoolAttributeRef("defaultButton", parameter.displaySetDefaultButton());
    reader.queryBoolAttributeRef("readOnly", parameter.readOnly());
    reader.queryIntAttributeRef("updateInterval", parameter.updateIntervalInMS());

    // specific parameters
    if (type == "double") {
        parameter.setType(AlgorithmParameterDescription::TYPE_DOUBLE);

        reader.queryRealAttributeRef("default", parameter.getDoubleDefaultValue());
        reader.queryRealAttributeRef("min", parameter.getDoubleMinValue());
        reader.queryRealAttributeRef("max", parameter.getDoubleMaxValue());
        reader.queryIntAttributeRef("precision", parameter.getDoublePrecision());
    } else if (type == "int") {
        parameter.setType(AlgorithmParameterDescription::TYPE_INT);

        reader.queryIntAttributeRef("default", parameter.getIntDefaultValue());
        reader.queryIntAttributeRef("min", parameter.getIntMinValue());
        reader.queryIntAttributeRef("max", parameter.getIntMaxValue());
    } else if (type == "list") {
        parameter.setType(AlgorithmParameterDescription::TYPE_LIST);

        parameter.getStringDefaultValue() = reader.queryStringAttribute("default");
    } else if (type == "bool" || type == "boolean") {
        parameter.setType(AlgorithmParameterDescription::TYPE_BOOL);

        reader.queryBoolAttributeRef("default", parameter.getBoolDefaultValue());
    } else {
        EPT_EXCEPT(EptException::ERR_INVALIDPARAMS, "Parameter type '" + type + "' is not supported.");
    }


    const std::string currentName = reader.name();

    while (reader.atEnd() == false) {
        reader.readNext();

        if (reader.isEndElement() && reader.name() == currentName) {break;}
        if (!reader.isStartElement()) {continue;}

        if (reader.name() == "label") {
            parameter.getLabel() = parseLanguageString(reader);
        } else if (reader.name() == "description") {
            parameter.getDescription() = parseLanguageString(reader);
        } else if (reader.name() == "entry") {
            std::string value = reader.queryStringAttribute("value");
            std::wstring label = parseLanguageString(reader);
            list.push_back(std::make_pair(value, label));
        }
    }



    EptAssert(parameter.getType() != AlgorithmParameterDescription::TYPE_UNSET, "The parameter type was not set");
    EptAssert(parameter.getID().empty() == false, "The parameter id is unset");

    return parameter;
}
