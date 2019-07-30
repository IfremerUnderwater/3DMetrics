#include "measurement_string.h"

MeasString::MeasString(const QString _fieldName) : MeasItem(_fieldName, 0)
{
}

// from JSon to object
void MeasString::decode(QJsonObject & _obj)
{
    m_val = _obj.value(fieldName()).toString();
}

// encode to JSon
void MeasString::encode(QJsonObject & _obj)
{
    _obj.insert(fieldName(), QJsonValue(m_val));
}

void MeasString::encodeASCII(QString &_string)
{
    _string = m_val;
}

void MeasString::encodeASCIILatLon(QString &_string)
{
    _string = m_val;
}
