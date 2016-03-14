#include "xmlwriterinterface.h"

#include "xmlfactory.h"

XmlWriterFactoryBase::XmlWriterFactoryBase(const std::string &id) :
  mId(id) {
    XmlFactory::getSingletonPtr()->registerWriter(this);
}
