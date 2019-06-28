#include <osg/Geode>
#include <osg/NodeVisitor>
#include <osg/Vec4>
#include <osg/Geometry>
#include <vector>
#include <limits>
#include "box_visitor.h"
using namespace std;

#include "math.h"

BoxVisitor::BoxVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
    m_x_min = numeric_limits<double>::max();
    m_x_max = numeric_limits<double>::min();
    m_y_min = numeric_limits<double>::max();
    m_y_max = numeric_limits<double>::min();
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
        m_sphere = geode.getBound();
        double x_min;
        double x_max;
        double y_min;
        double y_max;
        unsigned int num_drawables = geode.getNumDrawables();
        for( unsigned int i = 0; i < num_drawables; i++ )
        {
            // Use 'asGeometry' as its supposed to be faster than a dynamic_cast
            // every little saving counts
            osg::Geometry *current_geometry = geode.getDrawable(i)->asGeometry();

            // Only process if the drawable is geometry
            if ( current_geometry )
            {
                osg::Vec3Array *vertices = dynamic_cast<osg::Vec3Array*>(current_geometry->getVertexArray());
                // get the list of different geometry mode which were created
                osg::Geometry::PrimitiveSetList primitive_list = current_geometry->getPrimitiveSetList();
                // For our model 3D, we only have osg::PrimitiveSet::Triangle
                osg::PrimitiveSet *points = primitive_list[0];

                osg::Vec3f first_point;
                // we use set->index because we want to have the good value for all our triangles from the table of all points
                first_point = osg::Vec3f((* vertices)[points->index(0)].x(), (* vertices)[points->index(0)].y(), (* vertices)[points->index(0)].z()) ;
                x_min = first_point.x();
                x_max = first_point.x();
                y_min = first_point.y();
                y_max = first_point.y();
                for(unsigned int j = 1; j < points->getNumIndices(); j++)
                {
                    // we use set->index because we want to have the good value for all our triangles from the table of all points
                    if((* vertices)[points->index(j)].x() < x_min) x_min = (* vertices)[points->index(j)].x();
                    if((* vertices)[points->index(j)].y() < y_min) y_min = (* vertices)[points->index(j)].y();
                    if((* vertices)[points->index(j)].x() > x_max) x_max = (* vertices)[points->index(j)].x();
                    if((* vertices)[points->index(j)].y() > y_max) y_max = (* vertices)[points->index(j)].y();


                }
            }
        }
        if(x_min < m_x_min) m_x_min = x_min;
        if(y_min < m_y_min) m_y_min = y_min;
        if(x_max > m_x_max) m_x_max = x_max;
        if(y_max > m_y_max) m_y_max = y_max;

}
