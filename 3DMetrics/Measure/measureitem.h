#ifndef MEASUREITEM_H
#define MEASUREITEM_H

#include <QString>
#include <QJsonObject>

class MeasureItem
{
public:
    MeasureItem(const QString _fieldName);
    virtual ~MeasureItem();

    virtual QString type() = 0;

    // from JSon to object
    virtual void decode(QJsonObject & _obj) = 0;

    // encode to JSon
    virtual void encode(QJsonObject & _obj) = 0;

    QString fieldName() const { return m_fieldName; }

private:
    QString m_fieldName;
};

#endif // MEASUREITEM_H
