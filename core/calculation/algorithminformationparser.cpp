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

#include "algorithminformationparser.h"

#include <limits>
#include <locale>

#include "../system/eptexception.h"
#include "../system/log.h"
#include "../settings.h"
#include "../adapters/filemanager.h"

using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;

std::shared_ptr<const AlgorithmInformation> AlgorithmInformationParser::parse(const std::string &algorithmId) const {
    XMLDocument document;

    std::string fileContent(FileManager::getSingleton().getAlgorithmInformationFileContent(algorithmId));

    document.Parse(fileContent.c_str(), fileContent.size());

    if (document.Error()) {
        EPT_EXCEPT(EptException::ERR_CANNOT_READ_FROM_FILE, "Error while parsing algorithm information from file '" + fileContent + "'");
    }

    const XMLElement *root = document.FirstChildElement();

    if (!root) {
        EPT_EXCEPT(EptException::ERR_CANNOT_READ_FROM_FILE, "No root element found.");
    }

    const int year = root->IntAttribute("year");
    std::string author = root->Attribute("author");
    std::string name;
    std::string description;
    AlgorithmInformation::ParameterListType params;

    for (const XMLElement *child = root->FirstChildElement(); child; child = child->NextSiblingElement()) {
        if (strcmp(child->Name(), "name") == 0) {
            // name element
            name = parseLanguageString(child);
        } else if (strcmp(child->Name(), "description") == 0) {
            // description
            description = parseLanguageString(child);
        } else if (strcmp(child->Name(), "param") == 0) {
            // param
            params.push_back(parseAlgorithmParameter(child));
        }

    }

    return std::shared_ptr<const AlgorithmInformation>(
                new AlgorithmInformation(algorithmId, name, description, year, author, params));
}

std::string AlgorithmInformationParser::parseLanguageString(const tinyxml2::XMLElement *element) const {
    EptAssert(element, "XMLElement has to exist");

    std::string languageId = Settings::getSingleton().getUserLanguageId();

    std::string defaultLanguageString;

    for (const XMLElement *child = element->FirstChildElement(); child; child = child->NextSiblingElement()) {
        if (strcmp(child->Name(), "string") != 0) {
            EPT_EXCEPT(EptException::ERR_INVALIDPARAMS, "Only string names are expecten when parsing elements of type '"
                       + std::string(element->Name()) + "', but element with name '"
                       + std::string(child->Name()) + "' was given.");

        }
        if (child->Attribute("lang")) {
            if (languageId == child->Attribute("lang")) {
                return child->GetText();
            }
        } else {
            // default
            defaultLanguageString = child->GetText();

            if (languageId == "en") {
                // default language is en
                return defaultLanguageString;
            }
        }

        // not the desired string
    }

    // language string not found. Check if default string exist, then return this one
    if (defaultLanguageString.size() > 0) {
        LogD("Element '%s' has missing translation for '%s'", element->Value(), languageId.c_str());
        return defaultLanguageString;
    }

    EPT_EXCEPT(EptException::ERR_INVALIDPARAMS, "Element '" + std::string(element->Name()) + "' has no child elements with name 'string'");
}

AlgorithmParameter AlgorithmInformationParser::parseAlgorithmParameter(const tinyxml2::XMLElement *element) const {
    EptAssert(element, "XMLElement has to exist");

    const std::string id = element->Attribute("id");
    const std::string type = element->Attribute("type");

    std::string label, description;

    const XMLElement *labelElement = element->FirstChildElement("label");
    const XMLElement *descriptionElement = element->FirstChildElement("description");

    AlgorithmParameter parameter;

    parameter.getID() = id;

    if (labelElement) {
        parameter.getLabel() = parseLanguageString(labelElement);
    } else {
        EPT_EXCEPT(EptException::ERR_INVALIDPARAMS, "XMLElement 'label' missing while parsing a parameter element");
    }

    if (descriptionElement) {
        parameter.getDescription() = parseLanguageString(descriptionElement);
    } else {
        LogD("Desciption of parameter not set.");
    }

    // nonspecific parameters
    element->QueryBoolAttribute("lineEdit", &parameter.displayLineEdit());
    element->QueryBoolAttribute("spinBox", &parameter.displaySpinBox());
    element->QueryBoolAttribute("slider", &parameter.displaySlider());
    element->QueryBoolAttribute("defaultButton", &parameter.displaySetDefaultButton());
    element->QueryBoolAttribute("readOnly", &parameter.readOnly());
    element->QueryIntAttribute("updateInterval", &parameter.updateIntervalInMS());


    if (type == "double") {
        parameter.setType(AlgorithmParameter::TYPE_DOUBLE);

        element->QueryDoubleAttribute("default", &parameter.getDoubleDefaultValue());
        element->QueryDoubleAttribute("min", &parameter.getDoubleMinValue());
        element->QueryDoubleAttribute("max", &parameter.getDoubleMaxValue());
        element->QueryIntAttribute("precision", &parameter.getDoublePrecision());
    } else if (type == "int") {
        parameter.setType(AlgorithmParameter::TYPE_INT);

        element->QueryIntAttribute("default", &parameter.getIntDefaultValue());
        element->QueryIntAttribute("min", &parameter.getIntMinValue());
        element->QueryIntAttribute("max", &parameter.getIntMaxValue());
    } else if (type == "list") {
        parameter.setType(AlgorithmParameter::TYPE_LIST);

        parameter.getStringDefaultValue() = element->Attribute("default");

        AlgorithmParameter::StringParameterList &list = parameter.getStringList();

        for (const XMLElement *entry = element->FirstChildElement("entry"); entry;
             entry = entry->NextSiblingElement("entry")) {
            EptAssert(entry->Attribute("value"), "Value entry is required for string list parameter.");
            std::string value = entry->Attribute("value");
            std::string label = parseLanguageString(entry);
            list.push_back(std::make_pair(value, label));
        }
    } else {
        EPT_EXCEPT(EptException::ERR_INVALIDPARAMS, "Parameter type '" + type + "' is not supported.");
    }

    EptAssert(parameter.getType() != AlgorithmParameter::TYPE_UNSET, "The parameter type was not set");
    EptAssert(parameter.getID().empty() == false, "The parameter id is unset");

    return parameter;
}
