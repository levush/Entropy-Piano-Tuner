#include "xmlreaderinterface.h"

#include "xmlfactory.h"

XmlReaderFactoryBase::XmlReaderFactoryBase(const std::string &id) :
  mId(id) {
    XmlFactory::getSingletonPtr()->registerReader(this);
}
