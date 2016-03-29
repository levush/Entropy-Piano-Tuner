#ifndef XMLREADERINTERFACE_H
#define XMLREADERINTERFACE_H

#include "prerequisites.h"

#include "core/system/eptexception.h"

class EPT_EXTERN XmlReaderInterface
{
public:
    virtual bool openFile(const std::wstring &path) = 0;
    virtual bool openString(const std::wstring &contents) = 0;
    virtual void clear() = 0;

    virtual bool isOpen() = 0;

    virtual bool atEnd() = 0;
    virtual bool readNextStartElement() = 0;
    virtual bool isEndElement() = 0;
    virtual bool isStartElement() = 0;
    virtual bool readNext() = 0;
    virtual bool hasError() = 0;
    virtual std::string name() = 0;

    virtual bool hasAttribute(const std::string &id) = 0;
    void checkAttribute(const std::string &id) {
        if (!hasAttribute(id)) {
            EPT_EXCEPT(EptException::ERR_CANNOT_READ_FROM_FILE, ("Required attribute '" + id + "' does not exist.").c_str());
        }
    }

    virtual bool queryBoolAttribute(const std::string &id) = 0;

    void queryBoolAttributeRef(const std::string &id, bool &value) {
        value = queryBoolAttribute(id, static_cast<const bool>(value));
    }

    bool queryBoolAttribute(const std::string &id, bool defaultValue) {
        if (!hasAttribute(id)) {return defaultValue;}

        return queryBoolAttribute(id);
    }


    virtual int queryIntAttribute(const std::string &id) = 0;

    void queryIntAttributeRef(const std::string &id, int &value) {
        value = queryIntAttribute(id, static_cast<const int>(value));
    }

    int queryIntAttribute(const std::string &id, int defaultValue) {
        if (!hasAttribute(id)) {return defaultValue;}

        return queryIntAttribute(id);
    }


    virtual double queryRealAttribute(const std::string &id) = 0;

    void queryRealAttributeRef(const std::string &id, double &value) {
        value = queryRealAttribute(id, static_cast<const double>(value));
    }

    virtual double queryRealAttribute(const std::string &id, double defaultValue) {
        if (!hasAttribute(id)) {return defaultValue;}

        return queryRealAttribute(id);
    }



    virtual std::wstring queryWStringAttribute(const std::string &id) = 0;

    void queryWStringAttributeRef(const std::string &id, std::wstring &value) {
        value = queryWStringAttribute(id, static_cast<const std::wstring&>(value));
    }

    std::wstring queryWStringAttribute(const std::string &id, const std::wstring &defaultValue) {
        if (!hasAttribute(id)) {return defaultValue;}

        return queryWStringAttribute(id);
    }

    virtual std::string queryStringAttribute(const std::string &id) = 0;

    void queryStringAttributeRef(const std::string &id, std::string &value) {
        value = queryStringAttribute(id, static_cast<const std::string&>(value));
    }

    std::string queryStringAttribute(const std::string &id, const std::string &defaultValue) {
        if (!hasAttribute(id)) {return defaultValue;}

        return queryStringAttribute(id);
    }

    virtual std::wstring queryWStringText() = 0;
    virtual std::string queryStringText() = 0;
    virtual std::vector<double> queryDoubleVectorText() = 0;
    virtual double queryDoubleText() = 0;
    virtual int queryIntText() = 0;

};

typedef std::shared_ptr<XmlReaderInterface> XmlReaderInterfacePtr;

class EPT_EXTERN XmlReaderFactoryBase {
protected:
    XmlReaderFactoryBase(const std::string &mId);

public:
    virtual XmlReaderInterfacePtr createInstance() = 0;

    const std::string &getId() const {return mId;}

private:
    const std::string mId;
};

template <class T>
class XmlReaderFactory : private XmlReaderFactoryBase {
protected:
    XmlReaderFactory(const std::string &id) : XmlReaderFactoryBase(id) {}

    XmlReaderInterfacePtr createInstance() override final {
        return XmlReaderInterfacePtr(new T);
    }

private:
    static XmlReaderFactory mSingleton;
};

#define DECLARE_XML_READER_INTERFACE(clazz) \
    template <> XmlReaderFactory<clazz> XmlReaderFactory<clazz>::mSingleton(#clazz);

#endif // XMLREADERINTERFACE_H
