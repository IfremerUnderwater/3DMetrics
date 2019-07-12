#ifndef MEAS_LINE_H
#define MEAS_LINE_H

#include "measurement_item.h"
#include "measurement_type.h"
#include "point3d.h"

#include <QVector>

// store one line (polyline) - persistance in json
// fieldName: {
//    pts [
//      { x: val_x, y: val_y, z: val_z }
//    ],
//    length: length_m
// }
class MeasLine : public MeasItem
{
public:
    MeasLine(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode);

    virtual QString type() { return MeasType::value(MeasType::Line); }

    double length() { return m_length; }
    void computeLength();

    int nbPts() const { return m_array.length(); }
    QVector<Point3D> &getArray() { return m_array; }

    // TODO
    // add point
    // remove point

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

    // save / restore for edit
    void save();
    void restore();
    void cancel();

    // line color
    void setColor(osg::Vec4f _rgba) { m_color = _rgba; }

protected:
    QVector<Point3D> m_array;
    QVector<Point3D> m_array_save;
    double m_length;
    osg::Vec4f m_color;
};

#endif // MEAS_LINE_H
