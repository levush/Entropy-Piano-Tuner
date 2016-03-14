#ifndef XMLFACTORY_H
#define XMLFACTORY_H

#include <memory>
#include <vector>

#include "xmlreaderinterface.h"


class XmlFactory
{
private:
    XmlFactory();

public:
    static const std::unique_ptr<XmlFactory> &getSingletonPtr();

    void registerReader(XmlReaderFactoryBase *reader) {mReaderInterfaces.push_back(reader);}

    static XmlReaderInterfacePtr getDefaultReader();

private:
    static std::unique_ptr<XmlFactory> mSingleton;

    std::vector<XmlReaderFactoryBase*> mReaderInterfaces;
};

#endif // XMLFACTORY_H
