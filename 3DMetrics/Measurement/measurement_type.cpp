#include "measurement_type.h"
#include <QObject>

static QString values[MeasType::_Last] =
{
    QObject::tr("Line"),
    QObject::tr("Point"),
    QObject::tr("Area"),
    QObject::tr("Text"),
    QObject::tr("Category")};

QString MeasType::value(const type _t)
{
    if(_t >= 0 && _t < _Last)
        return values[_t];
    return QString();
}

MeasType::type MeasType::valueOf(QString _str)
{
    for(int i=0; i<MeasType::_Last; i++)
    {
        if(_str == values[i])
            return (MeasType::type)i;
    }
    return MeasType::_Last;
}


