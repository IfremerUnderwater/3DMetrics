#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Point>

#include "measurement_line.h"

#include <math.h>
#include <QJsonArray>

MeasureLine::MeasureLine(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode)
    : MeasureItem(_fieldName, _geode)
    , m_length(0)
    , m_color(0.0f,1.0f,0.0f,1.0f) // green by default
{

}

void MeasureLine::computeLength()
{
    double length = 0;

    for(int i=1; i<m_array.length(); i++)
    {
        double dx = m_array[i].x - m_array[i-1].x;
        double dy = m_array[i].y - m_array[i-1].y;
        double dz = m_array[i].z - m_array[i-1].z;

        length += sqrt(dx*dx + dy*dy + dz*dz);
    }

    m_length = length;
}


// from JSon to object
void MeasureLine::decode(QJsonObject & _obj)
{
    m_array.clear();

    QJsonObject p = _obj.value(fieldName()).toObject();
    m_length = p.value("length").toDouble();

    QJsonArray array = p.value("pts").toArray();
    for(int i=0; i<array.count(); i++)
    {
        QJsonObject op = array.at(i).toObject();
        Point3D p;
        p.decode(op);
        m_array.append(p);
    }
}


void MeasureLine::decode(QJsonObject & _obj, Point3D offset)
{
    m_array.clear();

    QJsonObject p = _obj.value(fieldName()).toObject();
    m_length = p.value("length").toDouble();

    QJsonArray array = p.value("pts").toArray();
    for(int i=0; i<array.count(); i++)
    {
        QJsonObject op = array.at(i).toObject();
        Point3D p;
        p.decode(op, offset);
        m_array.append(p);
    }
}

// encode to JSon
void MeasureLine::encode(QJsonObject & _obj)
{
    QJsonObject obj;

    // length
    obj.insert("length",QJsonValue(m_length));

    // pts
    QJsonArray array;
    for(int i=0; i<m_array.length(); i++)
    {
        Point3D p = m_array[i];
        QJsonObject val;
        p.encode(val);
        array.push_back(QJsonValue(val));
    }
    obj.insert("pts", array);

    // full object
    _obj.insert(fieldName(), obj);
}

void MeasureLine::encodeASCII(QString &_string)
{
    _string = QString::number(m_length,'f');
}

void MeasureLine::updateGeode()
{
    m_geode->removeDrawables(0, 2);

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
    // add a white color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    osg::Vec4 color(1.0f,0.75f,0.0f,1.0f);
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
    // add a green color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    //osg::Vec4 colorl(0.0f,1.0f,0.0f,1.0f);
    colorsl->push_back(m_color);

    // pass the color array to points geometry, note the binding to tell the geometry
    // that only use one color for the whole object.
    shape_line_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);

    // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
    // parameter passed to the DrawArrays constructor is the Primitive::Mode which
    // in this case is POINTS (which has the same value GL_POINTS), the second
    // parameter is the index position into the vertex array of the first point
    // to draw, and the third parameter is the number of points to draw.
    shape_line_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vertices->size()));

    m_geode->addDrawable(shape_line_drawable);
    m_geode->addDrawable(shape_point_drawable);
}


void MeasureLine::save()
{
    m_arraySave = m_array;
}

void MeasureLine::restore()
{
    m_array = m_arraySave;
    computeLength();
    updateGeode();
}

void MeasureLine::cancel()
{
    m_arraySave.clear();
}
