#ifndef ALGORITHMPARAMETERS_H
#define ALGORITHMPARAMETERS_H

#include <vector>
#include <string>
#include <map>
#include <memory>

class SingleAlgorithmParameters
{
public:
    SingleAlgorithmParameters(const std::string &algorithmName);

    const std::string &getAlgorithmName() const {return mAlgorithmName;}

    void loadDefaultParams();

    void setDoubleParameter(const std::string &s, double p) {mDoubleParameters[s] = p;}
    bool hasDoubleParameter(const std::string &s) const {return mDoubleParameters.count(s) > 0;}
    double getDoubleParameter(const std::string &s) const;
    const std::map<std::string, double> &getDoubleParameters() const {return mDoubleParameters;}

    void setIntParameter(const std::string &s, int i) {mIntParamters[s] = i;}
    bool hasIntParameter(const std::string &s) const {return mIntParamters.count(s) > 0;}
    int getIntParameter(const std::string &s) const;
    const std::map<std::string, int> &getIntParameters() const {return mIntParamters;}

    void setStringParameter(const std::string &id, const std::string &s) {mStringParameters[id] = s;}
    bool hasStringParameter(const std::string &id) const {return mStringParameters.count(id) > 0;}
    const std::string &getStringParameter(const std::string &id) const;
    const std::map<std::string, std::string> &getStringParameters() const {return mStringParameters;}

private:
    const std::string mAlgorithmName;

    std::map<std::string, double> mDoubleParameters;
    std::map<std::string, int> mIntParamters;
    std::map<std::string, std::string> mStringParameters;
};

typedef std::shared_ptr<SingleAlgorithmParameters> SingleAlgorithmParametersPtr;
typedef std::shared_ptr<const SingleAlgorithmParameters> ConstSingleAlgorithmParametersPtr;

class AlgorithmParameters
{
public:
    SingleAlgorithmParametersPtr getPreparedParameters(const std::string &id) const;
    SingleAlgorithmParametersPtr getOrCreate(const std::string &id) const;

    std::vector<SingleAlgorithmParametersPtr> &getParameters() {return mParameters;}
    const std::vector<SingleAlgorithmParametersPtr> &getParameters() const {return mParameters;}
private:
    // mutable, because getOrCreate may add parameters
    mutable std::vector<SingleAlgorithmParametersPtr> mParameters;
};


#endif // ALGORITHMPARAMETERS_H
