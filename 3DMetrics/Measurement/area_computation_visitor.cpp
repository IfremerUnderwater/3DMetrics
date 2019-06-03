#include <osg/Array>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/NodeVisitor>
#include <osg/Vec4>

#include "area_computation_visitor.h"

#include "math.h"

AreaComputationVisitor::AreaComputationVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
    m_area=0;
}

AreaComputationVisitor::~AreaComputationVisitor()
{
}

void AreaComputationVisitor::apply ( osg::Node &node )
{
    traverse(node);
}

void AreaComputationVisitor::apply( osg::Geode &geode )
{
    // Compute Total Area from a node (our model 3D)

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


            for(unsigned int j = 0; j < points->getNumIndices(); j=j+3)
            {
                osg::Vec3f point_a;
                osg::Vec3f point_b;
                osg::Vec3f point_c;

                // we use set->index because we want to have the good value for all our triangles from the table of all points
                point_a = osg::Vec3f((* vertices)[points->index(j)].x(), (* vertices)[points->index(j)].y(), (* vertices)[points->index(j)].z()) ;
                point_b = osg::Vec3f((* vertices)[points->index(j+1)].x(), (* vertices)[points->index(j+1)].y(), (* vertices)[points->index(j+1)].z()) ;
                point_c = osg::Vec3f((* vertices)[points->index(j+2)].x(), (* vertices)[points->index(j+2)].y(), (* vertices)[points->index(j+2)].z()) ;

                double AB = sqrt( pow((point_a.x()-point_b.x()),2) + pow((point_a.y()-point_b.y()),2) + pow((point_a.z()-point_b.z()),2) );
                double BC = sqrt( pow((point_b.x()-point_c.x()),2) + pow((point_b.y()-point_c.y()),2) + pow((point_b.z()-point_c.z()),2) );
                double AC = sqrt( pow((point_a.x()-point_c.x()),2) + pow((point_a.y()-point_c.y()),2) + pow((point_a.z()-point_c.z()),2) );

                double perimeter = (AB + BC + AC)/2;

                double area = sqrt(perimeter*(perimeter-AB)*(perimeter-BC)*(perimeter-AC));

                m_area = m_area + area;
            }

        }
    }
}
