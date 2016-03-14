#include "xmlfactory.h"

#include "core/system/eptexception.h"

std::unique_ptr<XmlFactory> XmlFactory::mSingleton;

XmlFactory::XmlFactory()
{

}

const std::unique_ptr<XmlFactory> &XmlFactory::getSingletonPtr() {
    if (!mSingleton) {
        mSingleton.reset(new XmlFactory());
    }

    return mSingleton;
}

XmlReaderInterfacePtr XmlFactory::getDefaultReader() {
    if (getSingletonPtr()->mReaderInterfaces.size() == 0) {
        EPT_EXCEPT(EptException::ERR_INTERNAL_ERROR, "No xml reader has been registered!");
    }

    return mSingleton->mReaderInterfaces.front()->createInstance();
}
