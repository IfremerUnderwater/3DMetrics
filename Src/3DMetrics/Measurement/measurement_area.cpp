#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Point>
#include "OSGWidget/osg_widget.h"
#include "OSGWidget/osg_widget_tool.h"
#include <GeographicLib/LocalCartesian.hpp>
#include "QPointF"

#include "measurement_area.h"

#include "Tools/geometry.h"

MeasArea::MeasArea(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode)
    : MeasLine(_fieldName, _geode), m_area(0)
{
    osg::Vec4 colorl(0.0f,1.0f,1.0f,1.0f); // cyan by default
    m_color = colorl;
}

// from JSon to object
void MeasArea::decode(QJsonObject & _obj)
{
    MeasLine::decode(_obj);
    QJsonObject area_json = _obj.value(fieldName()).toObject();
    m_area = area_json.value("area").toDouble();
}

void MeasArea::decode(QJsonObject & _obj, Point3D _offset)
{
    MeasLine::decode(_obj, _offset);
    QJsonObject area_json = _obj.value(fieldName()).toObject();
    m_area = area_json.value("area").toDouble();
}

// encode to JSon
void MeasArea::encode(QJsonObject & _obj)
{
    MeasLine::encode(_obj);
    QJsonObject area_json = _obj.value(fieldName()).toObject();
    area_json.insert("area",QJsonValue(m_area));
    _obj.insert(fieldName(), area_json);
}

// encode to ASCII
void MeasArea::encodeASCII(QString &_string)
{
    _string = QString::number(m_area,'f');
}

void MeasArea::encodeMeasASCIILatLon(QString &_string)
{
    _string = "area;" + QString::number(m_area,'f');

    QPointF ref_lat_lon;
    double ref_alt;
    OSGWidgetTool::instance()->getOSGWidget()->getGeoOrigin(ref_lat_lon, ref_alt);

    GeographicLib::LocalCartesian ltp_proj;
    ltp_proj.Reset(ref_lat_lon.x(), ref_lat_lon.y(), ref_alt);

    for(int i=0; i<m_array.length(); i++)
    {
        double lat,lon,alt;
        ltp_proj.Reverse(m_array[i].x, m_array[i].y, m_array[i].z, lat, lon, alt);

        _string = _string + ";"
                + QString::number(lat,'f',8) + ";"
                + QString::number(lon,'f',8) + ";"
                + QString::number(alt,'f',3);
    }
}

void MeasArea::encodeMeasASCIIXYZ(QString & _string)
{
    _string = "area;" + QString::number(m_area,'f');

    for(int i=0; i<m_array.length(); i++)
    {
        _string = _string + ";"
                + QString::number(m_array[i].x,'f',10) + ";"
                + QString::number(m_array[i].y,'f',10) + ";"
                + QString::number(m_array[i].z,'f',10);
    }
}

void MeasArea::computeLengthAndArea()
{
    computeLength();

    // compute area
    if(m_array.length() < 2)
    {
        m_area =0;
        return;
    }

    std::vector<Eigen::Vector3f> pt_area_data;

    // start at 0 - because we have not last point = first
    for(int i=0; i<m_array.length(); i++)
    {
        Eigen::Vector3f xyz;

        xyz[0] = m_array[i].x;
        xyz[1] = m_array[i].y;
        xyz[2] = m_array[i].z;

        pt_area_data.push_back(xyz);

    }
    // Fit points to plane
    std::pair<Eigen::Vector3f, Eigen::Vector3f> plane_coeffs = Geometry::bestPlaneFromPoints(pt_area_data);

    // Project points on plane
    std::vector<Eigen::Vector3f> proj_pt_area_data;
    Geometry::project3DPointsToPlane(plane_coeffs, pt_area_data, proj_pt_area_data);
    m_area = Geometry::projPointToArea(plane_coeffs, proj_pt_area_data);
}

void MeasArea::encodeShapefile(QString &_string)
{
    for(int i=0; i<m_array.length(); i++)
    {
        _string = _string
                + QString::number(m_array[i].x,'f',10) + "/"
                + QString::number(m_array[i].y, 'f', 10) + "/";
    }
}

void MeasArea::updateGeode()
{
    m_geode->removeDrawables(0, 3);

    if(m_array.length() == 0)
        return;

    //draw points
    osg::Geometry* shape_point_drawable = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array;

    for(int i=0; i<m_array.length(); i++)
    {
        osg::Vec3d point;
        point[0] = m_array[i].x;
        point[1] = m_array[i].y;
        point[2] = m_array[i].z;
        vertices->push_back(point);
    }

    // pass the created vertex array to the points geometry object.
    shape_point_drawable->setVertexArray(vertices);

    osg::Vec4Array* colors = new osg::Vec4Array;
    // add a yellow color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    osg::Vec4 color(1.0f,1.0f,0.0f,1.0f);
    colors->push_back(color);

    // pass the color array to points geometry, note the binding to tell the geometry
    // that only use one color for the whole object.
    shape_point_drawable->setColorArray(colors, osg::Array::BIND_OVERALL);

    // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
    // parameter passed to the DrawArrays constructor is the Primitive::Mode which
    // in this case is POINTS (which has the same value GL_POINTS), the second
    // parameter is the index position into the vertex array of the first point
    // to draw, and the third parameter is the number of points to draw.
    shape_point_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));
    shape_point_drawable->getOrCreateStateSet()->setAttribute(new osg::Point(4.f), osg::StateAttribute::ON);

    // line
    osg::Geometry* shape_line_drawable = new osg::Geometry();

    // pass the created vertex array to the points geometry object.
    shape_line_drawable->setVertexArray(vertices);

    osg::Vec4Array* colorsl = new osg::Vec4Array;
    // add a red color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    //osg::Vec4 colorl(1.0f,0.0f,0.0f,1.0f);
    colorsl->push_back(m_color);

    // pass the color array to points geometry, note the binding to tell the geometry
    // that only use one color for the whole object.
    shape_line_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);

    // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
    // parameter passed to the DrawArrays constructor is the Primitive::Mode which
    // in this case is POINTS (which has the same value GL_POINTS), the second
    // parameter is the index position into the vertex array of the first point
    // to draw, and the third parameter is the number of points to draw.
    shape_line_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,vertices->size()));

    // fill polygon
    osg::Geometry* shape_poly_drawable = new osg::Geometry();

    // pass the created vertex array to the points geometry object.
    shape_poly_drawable->setVertexArray(vertices);

    osg::Vec4Array* colorsp = new osg::Vec4Array;
    // add a red color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    osg::Vec4 colorp(m_color);
    colorp[3] = 0.25f;
    colorsp->push_back(colorp);

    // pass the color array to points geometry, note the binding to tell the geometry
    // that only use one color for the whole object.
    shape_poly_drawable->setColorArray(colorsp, osg::Array::BIND_OVERALL);

    // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
    // parameter passed to the DrawArrays constructor is the Primitive::Mode which
    // in this case is POINTS (which has the same value GL_POINTS), the second
    // parameter is the index position into the vertex array of the first point
    // to draw, and the third parameter is the number of points to draw.
    shape_poly_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,vertices->size()));

    m_geode->addDrawable(shape_poly_drawable);
    m_geode->addDrawable(shape_line_drawable);
    m_geode->addDrawable(shape_point_drawable);
}


void MeasArea::save()
{
    m_array_save = m_array;
}

void MeasArea::restore()
{
    m_array = m_array_save;
    computeLengthAndArea();
    updateGeode();
}

void MeasArea::cancel()
{
    m_array_save.clear();
}
