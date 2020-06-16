#ifndef MEAS_AREA_H
#define MEAS_AREA_H

#include "Measurement/measurement_item.h"
#include "Measurement/measurement_line.h"
#include "Measurement/measurement_type.h"
#include "OSGWidget/point3d.h"

//#include <QVector>

// store one area (polygon) - persistance in json
// fieldName: {
//    pts [
//      { x: val_x, y: val_y, z: val_z }
//    ],
//    length: length_m,
//    area: area_m2
// }
// NB : subclass of MeasLine
class MeasArea : public MeasLine
{
public:
    MeasArea(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode);

    virtual QString type() { return MeasType::value(MeasType::Area); }

    // from JSon to object
    virtual void decode(QJsonObject & _obj);
    virtual void decode(QJsonObject & _obj, Point3D _offset);

    // encode to JSon
    virtual void encode(QJsonObject & _obj);

    // encode to ASCII
    virtual void encodeASCII(QString & _string);
    virtual void encodeMeasASCIILatLon(QString & _string);
    virtual void encodeMeasASCIIXYZ(QString & _string);

    // encode to Shapefile
    virtual void encodeShapefile(QString & _string);

    double area() { return m_area; }
    void computeLengthAndArea();

    // create / update Geode
    virtual void updateGeode();

    // save / restore for edit
    void save();
    void restore();
    void cancel();

    // line color
    void setColor(osg::Vec4f _rgba) { m_color = _rgba; }

protected:
    double m_area;
    osg::Vec4f m_color;
};

#endif // MEAS_AREA_H
