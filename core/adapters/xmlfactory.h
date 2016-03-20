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
    static XmlFactory *getSingletonPtr();

    void registerReader(XmlReaderFactoryBase *reader) {mReaderInterfaces.push_back(reader);}
    void registerWriter(XmlWriterFactoryBase *writer) {mWriterInterfaces.push_back(writer);}

    static XmlReaderInterfacePtr getDefaultReader();
    static XmlWriterInterfacePtr getDefaultWriter();

private:
    std::vector<XmlReaderFactoryBase*> mReaderInterfaces;
    std::vector<XmlWriterFactoryBase*> mWriterInterfaces;
};

#endif // XMLFACTORY_H
