#include "measurement_pattern.h"
#include <QJsonArray>
#include <QJsonObject>

const QString MeasPattern::JSON_HEADER = "Measurement pattern";
const QString MeasPattern::JSON_HEADER_VALUE = "3DMetrics";
const QString MeasPattern::JSON_FIELD = "Fields";
const QString MeasPattern::JSON_FIELD_NAME = "Name";
const QString MeasPattern::JSON_FIELD_TYPE = "Type";

MeasPattern::MeasPattern()
{
    // init
    clear();
}

MeasPattern::MeasPattern(const MeasPattern&_other)
{
    m_doc = _other.m_doc;
}

MeasPattern::~ MeasPattern() {}

int MeasPattern::getNbFields()
{
    QJsonValue field_json = m_doc.object().value(JSON_FIELD);
    QJsonArray field_array_json = field_json.toArray();
    return field_array_json.count();
}

// field accessors
QString MeasPattern::fieldName(int _n)
{
    QJsonValue field_json = m_doc.object().value(JSON_FIELD);
    QJsonArray field_array_json = field_json.toArray();
    QJsonValue field_name_json = field_array_json.at(_n);
    return field_name_json.toObject().value(JSON_FIELD_NAME).toString();
}

MeasType::type MeasPattern::fieldType(int _n)
{
    return MeasType::valueOf(fieldTypeName(_n));
}

QString MeasPattern::fieldTypeName(int _n)
{
    QJsonValue field_json = m_doc.object().value(JSON_FIELD);
    QJsonArray field_array_json = field_json.toArray();
    QJsonValue field_type_json = field_array_json.at(_n);
    return field_type_json.toObject().value(JSON_FIELD_TYPE).toString();
}

// data
void MeasPattern::clear()
{
    QJsonArray field_array_json;
    QJsonObject field_info_json_obj;
    field_info_json_obj.insert(JSON_HEADER,JSON_HEADER_VALUE); // header
    field_info_json_obj.insert(JSON_FIELD,field_array_json); // empty array
    m_doc.setObject(field_info_json_obj);
}

void MeasPattern::addField(QString _name, MeasType::type _type)
{
    QJsonValue field_json = m_doc.object().value(JSON_FIELD);
    QJsonArray field_array_json = field_json.toArray();

    QJsonObject field_info_json_obj;
    field_info_json_obj.insert(JSON_FIELD_NAME,QJsonValue(_name));
    field_info_json_obj.insert(JSON_FIELD_TYPE,QJsonValue(MeasType::value(_type)));
    field_array_json.push_back(field_info_json_obj);

    QJsonObject rootobj = m_doc.object();
    rootobj.insert(JSON_FIELD,field_array_json);
    m_doc.setObject(rootobj);
}

bool MeasPattern::loadFromJson(QByteArray _ba)
{
    clear();

    QJsonDocument doc = QJsonDocument::fromJson(_ba);

    // checks
    if(doc.isNull() || doc.isEmpty())
        return false;

    // check "header"
    QJsonObject obj = doc.object();
    QString headval = obj.value(JSON_HEADER).toString();
    if( !(headval == JSON_HEADER_VALUE))
    {
        return false;
    }

    m_doc = doc;

    return true;
}
