#include "measurement_pattern.h"
#include <QJsonArray>
#include <QJsonObject>

const QString MeasurePattern::JSONHEADER = "Measurement pattern";
const QString MeasurePattern::JSONHEADERVALUE = "3DMetrics";
const QString MeasurePattern::JSONFIELD = "Fields";
const QString MeasurePattern::JSONFIELDNAME = "Name";
const QString MeasurePattern::JSONFIELDTYPE = "Type";

MeasurePattern::MeasurePattern()
{
    // init
    clear();
}

MeasurePattern::MeasurePattern(const MeasurePattern&_other)
{
    m_doc = _other.m_doc;
}

MeasurePattern::~ MeasurePattern() {}

int MeasurePattern::getNbFields()
{
    QJsonValue value = m_doc.object().value(JSONFIELD);
    QJsonArray array = value.toArray();
    return array.count();
}

// field accessors
QString MeasurePattern::fieldName(int _n)
{
    QJsonValue value = m_doc.object().value(JSONFIELD);
    QJsonArray array = value.toArray();
    QJsonValue v = array.at(_n);
    return v.toObject().value(JSONFIELDNAME).toString();
}

MeasureType::type MeasurePattern::fieldType(int _n)
{
    return MeasureType::valueOf(fieldTypeName(_n));
}

QString MeasurePattern::fieldTypeName(int _n)
{
    QJsonValue value = m_doc.object().value(JSONFIELD);
    QJsonArray array = value.toArray();
    QJsonValue v = array.at(_n);
    return v.toObject().value(JSONFIELDTYPE).toString();
}

// data
void MeasurePattern::clear()
{
    QJsonArray array;
    QJsonObject obj;
    obj.insert(JSONHEADER,JSONHEADERVALUE); // header
    obj.insert(JSONFIELD,array); // empty array
    m_doc.setObject(obj);
}

void MeasurePattern::addField(QString _name, MeasureType::type _type)
{
    QJsonValue v = m_doc.object().value(JSONFIELD);
    QJsonArray array = v.toArray();

    QJsonObject obj;
    obj.insert(JSONFIELDNAME,QJsonValue(_name));
    obj.insert(JSONFIELDTYPE,QJsonValue(MeasureType::value(_type)));
    array.push_back(obj);

    QJsonObject rootobj = m_doc.object();
    rootobj.insert(JSONFIELD,array);
    m_doc.setObject(rootobj);
}

bool MeasurePattern::loadFromJson(QByteArray ba)
{
    clear();

    QJsonDocument doc = QJsonDocument::fromJson(ba);

    // checks
    if(doc.isNull() || doc.isEmpty())
        return false;

    // check "header"
    QJsonObject obj = doc.object();
    QString headval = obj.value(JSONHEADER).toString();
    if( !(headval == JSONHEADERVALUE))
    {
        return false;
    }

    m_doc = doc;

    return true;
}
