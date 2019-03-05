#ifndef MEASURETYPE_H
#define MEASURETYPE_H

#include <QString>

class MeasureType
{
public:
    enum type {
        Line = 0,
        Point = 1,
        Area = 2,
        String = 3,
        Category = 4,
        _Last = 5 // must be the last item
    };

    static QString value(const type _t);
    static type valueOf(QString _str);
};

#endif // MEASURETYPE_H
