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

private :
    osg::BoundingBox m_box;
};

#endif // BOX_VISITOR_H
