#ifndef OBJECTMEANSIZEVISITOR_H
#define OBJECTMEANSIZEVISITOR_H

#include <osg/Geode>
#include <osg/NodeVisitor>

class ObjectMeanSizeVisitor : public osg::NodeVisitor
{
public :
    ObjectMeanSizeVisitor();
    virtual ~ObjectMeanSizeVisitor();

    virtual void apply( osg::Node &node );
    virtual void apply( osg::Geode &geode );

    double getMeanSize() const { return m_count > 0 ? m_totalZize / m_count : 0; }
    int getCount() const { return m_count; }

private :
    double m_totalZize;
    int m_count;
};


#endif // OBJECTMEANSIZEVISITOR_H
