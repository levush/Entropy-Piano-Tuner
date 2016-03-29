#include "xmlwriterinterface.h"

#include "xmlfactory.h"


void XmlWriterInterface::writeAttribute(const std::string &name, const char *value) {
    writeAttribute(name, std::string(value));
}

XmlWriterFactoryBase::XmlWriterFactoryBase(const std::string &id) :
  mId(id) {
    XmlFactory::getSingletonPtr()->registerWriter(this);
}
