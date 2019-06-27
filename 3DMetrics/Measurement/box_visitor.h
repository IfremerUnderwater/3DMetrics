#ifndef BOX_VISITOR_H
#define BOX_VISITOR_H

#include <osg/Array>
#include <osg/Geode>
#include <osg/NodeVisitor>

class BoxVisitor : public osg::NodeVisitor
{
public :
    BoxVisitor();
    virtual ~BoxVisitor();

    virtual void apply ( osg::Node &node );
    virtual void apply( osg::Geode &geode );

    osg::BoundingBox getBoundingBox() { return m_box; }
    osg::BoundingSphere getBoundingSphere() { return m_sphere; }
    double getxmin() { return m_x_min; }
    double getxmax() { return m_x_max; }
    double getymin() { return m_y_min; }
    double getymax() { return m_y_max; }

private :
    osg::BoundingBox m_box;
    osg::BoundingSphere m_sphere;
    double m_x_min;
    double m_x_max;
    double m_y_min;
    double m_y_max;
};

#endif // BOX_VISITOR_H
