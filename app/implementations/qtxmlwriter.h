#ifndef QTXMLWRITER_H
#define QTXMLWRITER_H

#include <QXmlStreamWriter>

#include "core/adapters/xmlwriterinterface.h"

class QtXmlWriter : public XmlWriterInterface
{
public:
    QtXmlWriter();

    virtual bool open() override final;
    virtual std::wstring close() override final;

    virtual bool hasError() override final;

    virtual void writeStartDocument() override final;
    virtual void writeEndDocument() override final;
    virtual void writeStartElement(const std::string &name) override final;
    virtual void writeEndElement() override final;

    virtual void writeAttribute(const std::string &name, const double &value) override final;
    virtual void writeAttribute(const std::string &name, const int &value) override final;
    virtual void writeAttribute(const std::string &name, const std::string &value) override final;
    virtual void writeAttribute(const std::string &name, const std::wstring &value) override final;
    virtual void writeAttribute(const std::string &name, const bool &value) override final;

    virtual void writeTextElement(const std::string &name, const std::string &value) override final;
    virtual void writeTextElement(const std::string &name, const std::wstring &value) override final;

    virtual void writeDoubleListElement(const std::string &name, const std::vector<double> &value, const std::string &separator = " ") override final;

    virtual void writeCharacters(const double &value) override final;
    virtual void writeCharacters(const std::string &value) override final;
    virtual void writeCharacters(const std::wstring &value) override final;

private:
    QString mString;
    QXmlStreamWriter mWriter;
};

#endif // QTXMLWRITER_H
