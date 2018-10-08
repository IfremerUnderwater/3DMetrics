#ifndef POINT
#define POINT

#include <QJsonObject>

struct Point3D
{
    Point3D() :x(0), y(0), z(0) {}

    double x;
    double y;
    double z;

    // from JSon to object
    void decode(QJsonObject & _obj);

    // encode to JSon
    void encode(QJsonObject & _obj);
};

#endif // POINT

