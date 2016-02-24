#include "algorithmparameters.h"
#include "core/calculation/calculationmanager.h"
#include "core/system/eptexception.h"

SingleAlgorithmParameters::SingleAlgorithmParameters(const std::string &algorithmName) :
    mAlgorithmName(algorithmName)
{

}

void SingleAlgorithmParameters::loadDefaultParams() {
    // load algorithm information from xml file
    // add all params with default value if not in algorithm description
    auto info = CalculationManager::getSingleton().loadAlgorithmInformation(getAlgorithmName());

    for (const AlgorithmParameter &param : info->getParameters()) {
        switch (param.getType()) {
        case AlgorithmParameter::TYPE_DOUBLE:
            if (hasDoubleParameter(param.getID())) {continue;}
            setDoubleParameter(param.getID(), param.getDoubleDefaultValue());
            break;
        case AlgorithmParameter::TYPE_INT:
            if (hasIntParameter(param.getID())) {continue;}
            setIntParameter(param.getID(), param.getIntDefaultValue());
            break;
        case AlgorithmParameter::TYPE_LIST:
            if (hasStringParameter(param.getID())) {continue;}
            setStringParameter(param.getID(), param.getStringDefaultValue());
            break;
        default:
            EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Parsing of default parameter not implemented");
            break;
        }
    }
}

double SingleAlgorithmParameters::getDoubleParameter(const std::string &s) const {
    EptAssert(hasDoubleParameter(s), "Parameter has to be in list before accessing.");
    return mDoubleParameters.at(s);
}

int SingleAlgorithmParameters::getIntParameter(const std::string &s) const {
    EptAssert(hasIntParameter(s), "Parameter has to be in list before accessing.");
    return mIntParamters.at(s);
}

const std::string &SingleAlgorithmParameters::getStringParameter(const std::string &id) const {
    EptAssert(hasStringParameter(id), "Parameter has to be in list before accessing.");
    return mStringParameters.at(id);
}

SingleAlgorithmParametersPtr AlgorithmParameters::getPreparedParameters(const std::string &id) const
{
    SingleAlgorithmParametersPtr a = getOrCreate(id);
    a->loadDefaultParams();

    return a;
}

SingleAlgorithmParametersPtr AlgorithmParameters::getOrCreate(const std::string &id) const
{
    for (SingleAlgorithmParametersPtr a : mParameters) {
        if (a->getAlgorithmName() == id) {
            return a;
        }
    }

    mParameters.push_back(std::make_shared<SingleAlgorithmParameters>(id));

    return mParameters.back();
}
