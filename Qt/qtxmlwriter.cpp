#include "qtxmlwriter.h"

#include <QTextStream>

DECLARE_XML_WRITER_INTERFACE(QtXmlWriter)

QtXmlWriter::QtXmlWriter() :
    XmlWriterInterface(),
    mString(),
    mWriter(&mString) {

    mWriter.setAutoFormatting(true);
}

bool QtXmlWriter::open() {
    return mWriter.hasError() == false;
}

std::wstring QtXmlWriter::close() {
    return mString.toStdWString();
}

bool QtXmlWriter::hasError() {
    return mWriter.hasError();
}

void QtXmlWriter::writeStartDocument() {
    mWriter.writeStartDocument();
}

void QtXmlWriter::writeEndDocument() {
    mWriter.writeEndDocument();
}

void QtXmlWriter::writeStartElement(const std::string &name) {
    mWriter.writeStartElement(QString::fromStdString(name));
}

void QtXmlWriter::writeEndElement() {
    mWriter.writeEndElement();
}

void QtXmlWriter::writeAttribute(const std::string &name, const double &value) {
    mWriter.writeAttribute(QString::fromStdString(name), QString::number(value));
}

void QtXmlWriter::writeAttribute(const std::string &name, const int &value) {
    mWriter.writeAttribute(QString::fromStdString(name), QString::number(value));
}

void QtXmlWriter::writeAttribute(const std::string &name, const std::string &value) {
    mWriter.writeAttribute(QString::fromStdString(name), QString::fromStdString(value));
}

void QtXmlWriter::writeAttribute(const std::string &name, const std::wstring &value) {
    mWriter.writeAttribute(QString::fromStdString(name), QString::fromStdWString(value));
}

void QtXmlWriter::writeAttribute(const std::string &name, const bool &value) {
    mWriter.writeAttribute(QString::fromStdString(name), value ? "1" : "0");
}

void QtXmlWriter::writeTextElement(const std::string &name, const std::string &value) {
    mWriter.writeTextElement(QString::fromStdString(name), QString::fromStdString(value));
}

void QtXmlWriter::writeTextElement(const std::string &name, const std::wstring &value) {
    mWriter.writeTextElement(QString::fromStdString(name), QString::fromStdWString(value));
}

void QtXmlWriter::writeDoubleListElement(const std::string &name, const std::vector<double> &values, const std::string &separator) {
    QString tsstring;
    QTextStream ts(&tsstring);
    ts.setCodec(mWriter.codec());
    ts.setLocale(QLocale(QLocale::English));
    for (size_t i = 0; i < values.size() - 1; ++i) {
        // all but last with space
        ts << values[i] << QString::fromStdString(separator);
    }
    // last without space
    ts << values.back();

    mWriter.writeTextElement(QString::fromStdString(name), tsstring);
}

void QtXmlWriter::writeCharacters(const double &value) {
    mWriter.writeCharacters(QString::number(value));
}

void QtXmlWriter::writeCharacters(const std::string &value) {
    mWriter.writeCharacters(QString::fromStdString(value));
}

void QtXmlWriter::writeCharacters(const std::wstring &value) {
    mWriter.writeCharacters(QString::fromStdWString(value));
}
