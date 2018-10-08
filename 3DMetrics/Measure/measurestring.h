#ifndef MEASURESTRING_H
#define MEASURESTRING_H

#include "Measure/measuretype.h"
#include "Measure/measureitem.h"

// store one string - persistance in json
// fieldName: value
class MeasureString : public MeasureItem
{
public:
    MeasureString(const QString _fieldName);

    virtual QString type() { return MeasureType::value(MeasureType::String); }

    QString value() const { return m_val; }
    void setValue(QString _val) { m_val = _val; }

    // from JSon to object
    virtual void decode(QJsonObject & _obj);

    // encode to JSon
    virtual void encode(QJsonObject & _obj);

private:
    QString m_val;
};

#endif // MEASURESTRING_H
