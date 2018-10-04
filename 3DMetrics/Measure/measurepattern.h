#ifndef MEASUREPATTERN_H
#define MEASUREPATTERN_H

#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMetaType>

#include "Measure/measuretype.h"

class MeasurePattern
{
public:
    MeasurePattern();
    MeasurePattern(const MeasurePattern&);
    ~ MeasurePattern();

    int getNbFields();

    // field accessors
    QString fieldName(int _n);
    MeasureType::type fieldType(int _n);
    QString fieldTypeName(int _n);

    // data
    void clear();
    void addField(QString _name, MeasureType::type _type);

    // load from json file
    bool loadFromJson(QByteArray ba);

    // json field names
    static const QString JSONHEADER; // = "Measure pattern";
    static const QString JSONHEADERVALUE; // = "3DMetrics";
    static const QString JSONFIELD; // = "Fields";
    static const QString JSONFIELDNAME; // = "Name";
    static const QString JSONFIELDTYPE; // = "Type";

    QJsonDocument get() { return m_doc; }

private:
    QJsonDocument m_doc;
};

// needed for using with QVariant
Q_DECLARE_METATYPE(MeasurePattern)

#endif // MEASUREPATTERN_H
