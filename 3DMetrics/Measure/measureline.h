#ifndef MEASURELINE_H
#define MEASURELINE_H

#include "Measure/measureitem.h"
#include "Measure/measuretype.h"
#include "Measure/point3d.h"

#include <QVector>

// store one line (polyline) - persistance in json
// fieldName: {
//    pts [
//      { x: val_x, y: val_y, z: val_z }
//    ],
//    length: length_m
// }
class MeasureLine : public MeasureItem
{
public:
    MeasureLine(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode);

    virtual QString type() { return MeasureType::value(MeasureType::Line); }

    double length() { return m_length; }
    void computeLength();

    int nbPts() const { return m_array.length(); }
    QVector<Point3D> &getArray() { return m_array; }

    // TODO
    // add point
    // remove point

    // from JSon to object
    virtual void decode(QJsonObject & _obj);

    // encode to JSon
    virtual void encode(QJsonObject & _obj);

    // create / update Geode
    virtual void updateGeode();

    // save / restore for edit
    void save();
    void restore();
    void cancel();

protected:
    QVector<Point3D> m_array;
    QVector<Point3D> m_arraySave;
    double m_length;
};

#endif // MEASURELINE_H
