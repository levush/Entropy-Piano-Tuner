#ifndef QTXMLREADER_H
#define QTXMLREADER_H

#include <QXmlStreamReader>

#include "core/adapters/xmlreaderinterface.h"

class QtXmlReader : public XmlReaderInterface
{
public:
    virtual bool openFile(const std::wstring &path) override final;
    virtual bool openString(const std::wstring &contents) override final;
    virtual void clear() override final;

    virtual bool isOpen() override final;

    virtual bool atEnd() override final;
    virtual bool readNextStartElement() override final;
    virtual bool isEndElement() override final;
    virtual bool isStartElement() override final;
    virtual bool readNext() override final;
    virtual bool hasError() override final;
    virtual std::string name() override final;

    virtual bool hasAttribute(const std::string &id) override final;

    virtual bool queryBoolAttribute(const std::string &id) override final;
    virtual int queryIntAttribute(const std::string &id) override final;
    virtual double queryRealAttribute(const std::string &id) override final;
    virtual std::wstring queryWStringAttribute(const std::string &id) override final;
    virtual std::string queryStringAttribute(const std::string &id) override final;
    virtual std::wstring queryElementText() override final;
    virtual std::vector<double> queryDoubleVectorText() override final;


private:
    QXmlStreamReader mReader;
};

#endif // QTXMLREADER_H
