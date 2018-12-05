#include "measuretype.h"
#include <QObject>

static QString values[MeasureType::_Last] =
{
    QObject::tr("Line"),
    QObject::tr("Point"),
    QObject::tr("Area"),
    QObject::tr("String")};

QString MeasureType::value(const type _t)
{
    if(_t >= 0 && _t < _Last)
        return values[_t];
    return QString();
}

MeasureType::type MeasureType::valueOf(QString _str)
{
    for(int i=0; i<MeasureType::_Last; i++)
    {
        if(_str == values[i])
            return (MeasureType::type)i;
    }
    return MeasureType::_Last;
}


