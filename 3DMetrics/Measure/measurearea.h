#ifndef MEASUREAREA_H
#define MEASUREAREA_H

#include "Measure/measureitem.h"
#include "Measure/measuretype.h"
#include "Measure/point3d.h"

#include <QVector>

// store one area (polygon) - persistance in json
// fieldName: {
//    pts [
//      { x: val_x, y: val_y, z: val_z }
//    ],
//    length: length_m,
//    area: area_m2
// }
class MeasureArea : public MeasureItem
{
public:
    MeasureArea(const QString _fieldName);

    virtual QString type() { return MeasureType::value(MeasureType::Perimeter); }

    // from JSon to object
    virtual void decode(QJsonObject & _obj);

    // encode to JSon
    virtual void encode(QJsonObject & _obj);

    int nbPts() const { return m_array.length(); }
    QVector<Point3D> &getArray() { return m_array; }

    double length() { return m_length; }
    double area() { return m_area; }
    void computeLengthAndArea();

private:
    double m_length;
    double m_area;
    QVector<Point3D> m_array;
};

#endif // MEASUREAREA_H
