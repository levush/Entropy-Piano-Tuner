#ifndef XMLFACTORY_H
#define XMLFACTORY_H

#include <memory>
#include <vector>

#include "xmlreaderinterface.h"
#include "xmlwriterinterface.h"


class XmlFactory
{
private:
    XmlFactory();

public:
    static const std::unique_ptr<XmlFactory> &getSingletonPtr();

    void registerReader(XmlReaderFactoryBase *reader) {mReaderInterfaces.push_back(reader);}
    void registerWriter(XmlWriterFactoryBase *writer) {mWriterInterfaces.push_back(writer);}

    static XmlReaderInterfacePtr getDefaultReader();
    static XmlWriterInterfacePtr getDefaultWriter();

private:
    static std::unique_ptr<XmlFactory> mSingleton;

    std::vector<XmlReaderFactoryBase*> mReaderInterfaces;
    std::vector<XmlWriterFactoryBase*> mWriterInterfaces;
};

#endif // XMLFACTORY_H
