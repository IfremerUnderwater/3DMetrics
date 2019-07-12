#ifndef MEAS_POINT_H
#define MEAS_POINT_H

#include "measurement_item.h"
#include "measurement_type.h"
#include "point3d.h"

// store one point - persistance in json
// fieldName: { x: val_x, y: val_y, z: val_z }
class MeasPoint : public MeasItem
{
public:
    MeasPoint(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode);

    virtual QString type() { return MeasType::value(MeasType::Point); }

    double x() const { return m_point.x; }
    void setX(double _x) { m_point.x = _x; }

    double y() const { return m_point.y; }
    void setY(double _y) { m_point.y = _y; }

    double z() const { return m_point.z; }
    void setZ(double _z) { m_point.z = _z; }

    Point3D point() const { return m_point; }
    void setP(Point3D _point) { m_point = _point; }

    // from JSon to object
    virtual void decode(QJsonObject & _obj);
    virtual void decode(QJsonObject & _obj, Point3D _offset);

    // encode to JSon
    virtual void encode(QJsonObject & _obj);

    // encode to ASCII
    virtual void encodeASCII(QString & _string);

    // encode to Shapefile
    virtual void encodeShapefile(QString & _string);

    // create / update Geode
    virtual void updateGeode();

    void setColor(osg::Vec4f _rgba) { m_color = _rgba; }

protected:
    Point3D m_point;
    osg::Vec4f m_color;
};

#endif // MEAS_POINT_H
