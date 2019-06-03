#ifndef MEAS_TYPE_H
#define MEAS_TYPE_H

#include <QString>

class MeasType
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

#endif // MEAS_TYPE_H
