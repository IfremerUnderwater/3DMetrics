#include "measurement_category.h"

MeasCategory::MeasCategory(const QString _fieldName) : MeasItem(_fieldName, 0)
{
}

// from JSon to object
void MeasCategory::decode(QJsonObject & _obj)
{
    m_val = _obj.value(fieldName()).toString();
}

// encode to JSon
void MeasCategory::encode(QJsonObject & _obj)
{
    _obj.insert(fieldName(), QJsonValue(m_val));
}

// encode to ASCII
void MeasCategory::encodeASCII(QString &_string)
{
    _string = m_val;
}

void MeasCategory::encodeASCIILatLon(QString &_string)
{
    _string = m_val;
}


void MeasCategory::encodeShapefile(QString &_string)
{

}
