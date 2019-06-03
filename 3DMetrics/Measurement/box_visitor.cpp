#include <osg/Geode>
#include <osg/NodeVisitor>

#include "box_visitor.h"

#include "math.h"

BoxVisitor::BoxVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
}

BoxVisitor::~BoxVisitor()
{
}

void BoxVisitor::apply ( osg::Node &node )
{
    traverse(node);
}

void BoxVisitor::apply( osg::Geode &geode )
{
        m_box = geode.getBoundingBox();
}
