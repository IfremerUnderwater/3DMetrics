#include <osg/Geode>
#include <osg/NodeVisitor>
#include <osg/Vec4>
#include <osg/Geometry>
#include <vector>
#include "box_visitor.h"
using namespace std;

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
        m_sphere = geode.getBound();

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
                m_x_min = first_point.x();
                m_x_max = first_point.x();
                m_y_min = first_point.y();
                m_y_max = first_point.y();
                for(unsigned int j = 1; j < points->getNumIndices(); j++)
                {
                    osg::Vec3f point_a;
                    // we use set->index because we want to have the good value for all our triangles from the table of all points
                    point_a = osg::Vec3f((* vertices)[points->index(j)].x(), (* vertices)[points->index(j)].y(), (* vertices)[points->index(j)].z()) ;
                    if(point_a.x() < m_x_min) m_x_min = point_a.x();
                    if(point_a.y() < m_y_min) m_y_min = point_a.y();
                    if(point_a.x() > m_x_max) m_x_max = point_a.x();
                    if(point_a.y() > m_y_max) m_y_max = point_a.y();


                }
            }
        }
}
