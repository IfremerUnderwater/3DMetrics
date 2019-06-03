#ifndef MEAS_PATTERN_H
#define MEAS_PATTERN_H

#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMetaType>

#include "measurement_type.h"

class MeasPattern
{
public:
    MeasPattern();
    MeasPattern(const MeasPattern&);
    ~ MeasPattern();

    int getNbFields();

    // field accessors
    QString fieldName(int _n);
    MeasType::type fieldType(int _n);
    QString fieldTypeName(int _n);

    // data
    void clear();
    void addField(QString _name, MeasType::type _type);

    // load from json file
    bool loadFromJson(QByteArray _ba);

    // json field names
    // header
    static const QString JSON_HEADER; // = "Measurement pattern";
    static const QString JSON_HEADER_VALUE; // = "3DMetrics";
    // fields
    static const QString JSON_FIELD; // = "Fields";
    static const QString JSON_FIELD_NAME; // = "Name";
    static const QString JSON_FIELD_TYPE; // = "Type";

    QJsonDocument get() { return m_doc; }
    void set(QJsonDocument _doc) { m_doc = _doc; }

private:
    QJsonDocument m_doc;
};

// needed for using with QVariant
Q_DECLARE_METATYPE(MeasPattern)

#endif // MEAS_PATTERN_H
