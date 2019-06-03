
#ifndef AREA_COMPUTATION_VISITOR_H
#define AREA_COMPUTATION_VISITOR_H

#include <osg/Array>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/NodeVisitor>
#include <osg/Vec4>

class AreaComputationVisitor : public osg::NodeVisitor
{
public :
    AreaComputationVisitor();
    virtual ~AreaComputationVisitor();

    virtual void apply ( osg::Node &node );
    virtual void apply( osg::Geode &geode );

    double getArea() { return m_area; }

private :
    double m_area;
};

#endif // AREA_COMPUTATION_VISITOR_H
