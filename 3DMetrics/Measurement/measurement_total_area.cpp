#include <osg/Array>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/NodeVisitor>
#include <osg/Vec4>

#include "measurement_total_area.h"

#include "math.h"

MeasurementTotalArea::MeasurementTotalArea() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
    m_area=0;
}

MeasurementTotalArea::~MeasurementTotalArea()
{
}

void MeasurementTotalArea::apply ( osg::Node &node )
{
    traverse(node);
}

void MeasurementTotalArea::apply( osg::Geode &geode )
{
    // Compute Total Area from a node (our model 3D)

    unsigned int m_numDrawables = geode.getNumDrawables();
    for( unsigned int geodeIdx = 0; geodeIdx < m_numDrawables; geodeIdx++ )
    {
        // Use 'asGeometry' as its supposed to be faster than a dynamic_cast
        // every little saving counts
        osg::Geometry *curGeom = geode.getDrawable(geodeIdx)->asGeometry();

        // Only process if the drawable is geometry
        if ( curGeom )
        {
            osg::Vec3Array *vertices = dynamic_cast<osg::Vec3Array*>(curGeom->getVertexArray());
            // get the list of different geometry mode which were created
            osg::Geometry::PrimitiveSetList primitiveList = curGeom->getPrimitiveSetList();
            // For our model 3D, we only have osg::PrimitiveSet::Triangle
            osg::PrimitiveSet *set = primitiveList[0];


            for(int y = 0; y < set->getNumIndices(); y=y+3)
            {
                osg::Vec3f pointA;
                osg::Vec3f pointB;
                osg::Vec3f pointC;

                // we use set->index because we want to have the good value for all our triangles from the table of all points
                pointA = osg::Vec3f((* vertices)[set->index(y)].x(), (* vertices)[set->index(y)].y(), (* vertices)[set->index(y)].z()) ;
                pointB = osg::Vec3f((* vertices)[set->index(y+1)].x(), (* vertices)[set->index(y+1)].y(), (* vertices)[set->index(y+1)].z()) ;
                pointC = osg::Vec3f((* vertices)[set->index(y+2)].x(), (* vertices)[set->index(y+2)].y(), (* vertices)[set->index(y+2)].z()) ;

                double AB = sqrt( pow((pointA.x()-pointB.x()),2) + pow((pointA.y()-pointB.y()),2) + pow((pointA.z()-pointB.z()),2) );
                double BC = sqrt( pow((pointB.x()-pointC.x()),2) + pow((pointB.y()-pointC.y()),2) + pow((pointB.z()-pointC.z()),2) );
                double AC = sqrt( pow((pointA.x()-pointC.x()),2) + pow((pointA.y()-pointC.y()),2) + pow((pointA.z()-pointC.z()),2) );

                double perimeter = (AB + BC + AC)/2;

                double area = sqrt(perimeter*(perimeter-AB)*(perimeter-BC)*(perimeter-AC));

                m_area = m_area + area;
            }

        }
        m_box = geode.getBoundingBox();
    }
}
