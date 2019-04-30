
#ifndef MEASUREMENTTOTALAREA_H
#define MEASUREMENTTOTALAREA_H

#include <osg/Array>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/NodeVisitor>
#include <osg/Vec4>

class MeasurementTotalArea : public osg::NodeVisitor
{
public :
    MeasurementTotalArea();
    virtual ~MeasurementTotalArea();

    virtual void apply ( osg::Node &node );
    virtual void apply( osg::Geode &geode );

    double getArea() { return m_area; }
    osg::BoundingBox getBoundingBox() { return m_box; }

private :
    double m_area;
    osg::BoundingBox m_box;
};

#endif // MEASUREMENTTOTALAREA_H
