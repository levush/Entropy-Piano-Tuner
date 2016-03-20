#include "xmlfactory.h"

#include "core/system/eptexception.h"

XmlFactory::XmlFactory()
{

}

XmlFactory *XmlFactory::getSingletonPtr() {
    static XmlFactory mSingleton;
    return &mSingleton;
}

XmlReaderInterfacePtr XmlFactory::getDefaultReader() {
    if (getSingletonPtr()->mReaderInterfaces.size() == 0) {
        EPT_EXCEPT(EptException::ERR_INTERNAL_ERROR, "No xml reader has been registered!");
    }

    return getSingletonPtr()->mReaderInterfaces.front()->createInstance();
}

XmlWriterInterfacePtr XmlFactory::getDefaultWriter() {
    if (getSingletonPtr()->mWriterInterfaces.size() == 0) {
        EPT_EXCEPT(EptException::ERR_INTERNAL_ERROR, "No xml writer has been registered!");
    }

    return getSingletonPtr()->mWriterInterfaces.front()->createInstance();
}
