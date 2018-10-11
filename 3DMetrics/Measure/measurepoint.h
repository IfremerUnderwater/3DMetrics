#ifndef MEASUREPOINT_H
#define MEASUREPOINT_H

#include "Measure/measureitem.h"
#include "Measure/measuretype.h"
#include "Measure/point3d.h"

// store one point - persistance in json
// fieldName: { x: val_x, y: val_y, z: val_z }
class MeasurePoint : public MeasureItem
{
public:
    MeasurePoint(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode);

    virtual QString type() { return MeasureType::value(MeasureType::Point); }

    double x() const { return m_p.x; }
    void setX(double _x) { m_p.x = _x; }

    double y() const { return m_p.y; }
    void setY(double _y) { m_p.y = _y; }

    double z() const { return m_p.z; }
    void setZ(double _z) { m_p.z = _z; }

    Point3D p() const { return m_p; }
    void setP(Point3D _p) { m_p = _p; }

    // from JSon to object
    virtual void decode(QJsonObject & _obj);

    // encode to JSon
    virtual void encode(QJsonObject & _obj);

private:
    Point3D m_p;
};

#endif // MEASUREPOINT_H
