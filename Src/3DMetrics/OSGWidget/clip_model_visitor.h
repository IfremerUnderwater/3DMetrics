#ifndef CLIPMODELVISITOR_H
#define CLIPMODELVISITOR_H

#include <osg/Geode>
#include <osg/NodeVisitor>

class ClipModelVisitor : public osg::NodeVisitor
{
public:
    ClipModelVisitor();
    ClipModelVisitor(osg::BoundingBox _boundingBox);
    virtual ~ClipModelVisitor();

    virtual void apply( osg::Node &node );
    virtual void apply( osg::Geode &geode );

    void setBoundingBox(osg::BoundingBox _boundingBox);

    osg::ref_ptr<osg::Node> getClippedNode() { return m_clippedNode; }

private :
    osg::ref_ptr<osg::Node> m_clippedNode;
    osg::BoundingBox m_boundingBox;
};


#endif // CLIPMODELVISITOR_H
