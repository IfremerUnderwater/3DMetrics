#ifndef MEAS_STRING_H
#define MEAS_STRING_H

#include "measurement_type.h"
#include "measurement_item.h"

// store one string - persistance in json
// fieldName: value
class MeasString : public MeasItem
{
public:
    MeasString(const QString _fieldName);

    virtual QString type() { return MeasType::value(MeasType::String); }

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

#endif // MEAS_STRING_H
