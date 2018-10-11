#include "measurestring.h"

MeasureString::MeasureString(const QString _fieldName) : MeasureItem(_fieldName, 0)
{
}

// from JSon to object
void MeasureString::decode(QJsonObject & _obj)
{
    m_val = _obj.value(fieldName()).toString();
}

// encode to JSon
void MeasureString::encode(QJsonObject & _obj)
{
    _obj.insert(fieldName(), QJsonValue(m_val));
}
