#ifndef MEAS_CATEGORY_H
#define MEAS_CATEGORY_H

#include "measurement_type.h"
#include "measurement_item.h"

// store a category - persistance in json
// fieldName: value
class MeasCategory : public MeasItem
{
public:
    MeasCategory(const QString _fieldName);

    virtual QString type() { return MeasType::value(MeasType::Category); }

    QString value() const { return m_val; }
    void setValue(QString _val) { m_val = _val; }

    // from JSon to object
    virtual void decode(QJsonObject & _obj);

    // encode to JSon
    virtual void encode(QJsonObject & _obj);

    // encode to ASCII
    virtual void encodeASCII(QString & _string);

    virtual void updateGeode() {}

private:
    QString m_val;
};

#endif // MEAS_CATEGORY_H
