#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Point>
#include "OSGWidget/osg_widget_tool.h"
#include "OSGWidget/osg_widget.h"
#include <GeographicLib/LocalCartesian.hpp>
#include "QPointF"

#include "measurement_point.h"

#include <QDebug>


MeasurePoint::MeasurePoint(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode)
    : MeasureItem(_fieldName, _geode),
      m_p(),
      m_color(0.0f,0.0f,1.0f,1.0f) // blue by default
{

}

// from JSon to object
void MeasurePoint::decode(QJsonObject & _obj)
{
  QJsonObject p = _obj.value(fieldName()).toObject();
  m_p.decode(p);
}

void MeasurePoint::decode(QJsonObject & _obj, Point3D offset)
{
  QJsonObject p = _obj.value(fieldName()).toObject();
  m_p.decode(p, offset);
}

// encode to JSon
void MeasurePoint::encode(QJsonObject & _obj)
{
    QJsonObject p;
    m_p.encode(p);
    _obj.insert(fieldName(), QJsonValue(p));
}

void MeasurePoint::encodeASCII(QString &_string)
{
    QPointF ref_lat_lon;
    double ref_depth;
    OSGWidgetTool::instance()->getOSGWidget()->getGeoOrigin(ref_lat_lon, ref_depth);

    GeographicLib::LocalCartesian ltp_proj;
    ltp_proj.Reset(ref_lat_lon.x(), ref_lat_lon.y(), ref_depth);

    double lat,lon,depth;
    ltp_proj.Reverse(m_p.x, m_p.y, m_p.z, lat, lon, depth);

    _string = "lat=" + QString::number(lat,'f',8)+
            "\tlon="+ QString::number(lon,'f',8)+
            "\tdepth="+ QString::number(depth,'f',3);
}

void MeasurePoint::updateGeode()
{
    osg::Vec3d point;
    point[0] = m_p.x;
    point[1] = m_p.y;
    point[2] = m_p.z;
    // create point in geode
    // point
    osg::Geometry* shape_point_drawable = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array;
    vertices->push_back(point);

    // pass the created vertex array to the points geometry object.
    shape_point_drawable->setVertexArray(vertices);

    osg::Vec4Array* colors = new osg::Vec4Array;
    // add a white color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    //osg::Vec4 color(0.0f,0.0f,1.0f,1.0f);
    colors->push_back(m_color);

    // pass the color array to points geometry, note the binding to tell the geometry
    // that only use one color for the whole object.
    shape_point_drawable->setColorArray(colors, osg::Array::BIND_OVERALL);

    // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
    // parameter passed to the DrawArrays constructor is the Primitive::Mode which
    // in this case is POINTS (which has the same value GL_POINTS), the second
    // parameter is the index position into the vertex array of the first point
    // to draw, and the third parameter is the number of points to draw.
    shape_point_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));

    // fixed size points
    shape_point_drawable->getOrCreateStateSet()->setAttribute(new osg::Point(4.f), osg::StateAttribute::ON);

    m_geode->removeDrawables(0);
    m_geode->addDrawable(shape_point_drawable);
}
