#include "measurement_category.h"

MeasureCategory::MeasureCategory(const QString _fieldName) : MeasureItem(_fieldName, 0)
{
}

// from JSon to object
void MeasureCategory::decode(QJsonObject & _obj)
{
    m_val = _obj.value(fieldName()).toString();
}

// encode to JSon
void MeasureCategory::encode(QJsonObject & _obj)
{
    _obj.insert(fieldName(), QJsonValue(m_val));
}
