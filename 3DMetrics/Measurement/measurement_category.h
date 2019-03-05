#ifndef MEASURECATEGORY_H
#define MEASURECATEGORY_H

#include "measurement_type.h"
#include "measurement_item.h"

// store a category - persistance in json
// fieldName: value
class MeasureCategory : public MeasureItem
{
public:
    MeasureCategory(const QString _fieldName);

    virtual QString type() { return MeasureType::value(MeasureType::Category); }

    QString value() const { return m_val; }
    void setValue(QString _val) { m_val = _val; }

    // from JSon to object
    virtual void decode(QJsonObject & _obj);

    // encode to JSon
    virtual void encode(QJsonObject & _obj);

    virtual void updateGeode() {}

private:
    QString m_val;
};

#endif // MEASURECATEGORY_H
