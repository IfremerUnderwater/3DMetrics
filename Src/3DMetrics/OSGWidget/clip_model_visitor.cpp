#include "clip_model_visitor.h"
#include <osg/Geode>
#include <osg/Geometry>
#include <osgUtil/SmoothingVisitor>
#include <osg/StateSet>

ClipModelVisitor::ClipModelVisitor(): osg::NodeVisitor(TRAVERSE_ALL_CHILDREN), m_margin(0)
{
    m_clippedNode = new osg::Group;
}

ClipModelVisitor::ClipModelVisitor(osg::BoundingBox _boundingBox, double _margin) : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
    m_boundingBox(_boundingBox), m_margin(_margin)
{
    m_clippedNode = new osg::Group;
}

ClipModelVisitor::~ClipModelVisitor()
{
}

void ClipModelVisitor::apply(osg::Node &node)
{
    //if (node.getStateSet()) apply(*node.getStateSet());

    traverse(node);
}

void ClipModelVisitor::apply(osg::Geode &geode)
{
    //if (geode.getStateSet()) apply(*geode.getStateSet());

    if(!m_boundingBox.valid())
        return;

    // some optimizations
    const double XMIN = m_boundingBox.xMin() - m_margin;
    const double XMAX = m_boundingBox.xMax() + m_margin;
    const double YMIN = m_boundingBox.yMin() - m_margin;
    const double YMAX = m_boundingBox.yMax() + m_margin;
    const double ZMIN = m_boundingBox.zMin() - m_margin;
    const double ZMAX = m_boundingBox.zMax() + m_margin;

    unsigned int num_drawables = geode.getNumDrawables();
    for( unsigned int i = 0; i < num_drawables; i++ )
    {
        TextureSet ts;
        osg::StateSet *stateSet = geode.getDrawable(i)->getStateSet();
        if (stateSet != nullptr)
        {
            // search for the existence of any texture object attributes
            for(unsigned int i=0;i<stateSet->getTextureAttributeList().size();++i)
            {
                osg::Texture* texture = dynamic_cast<osg::Texture*>(stateSet->getTextureAttribute(i,osg::StateAttribute::TEXTURE));
                if (texture)
                {
                    ts.insert(texture);
                }
            }
        }


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

            for(unsigned int j = 0; j < primitive_list.size(); j++)
            {

                osg::PrimitiveSet *primitive_set = primitive_list[j];
                unsigned int inc=0;
                if (primitive_set->getMode() == osg::PrimitiveSet::TRIANGLE_STRIP)
                {
                    inc = 1;
                }
                else if (primitive_set->getMode() == osg::PrimitiveSet::TRIANGLES)
                {
                    inc = 3;
                }
                else
                {
                    return;
                }
                if(primitive_set->getNumIndices() < 3)
                {
                    // avoid segv
                    continue;
                }

                osg::ref_ptr<osg::Vec3Array> outarray = new osg::Vec3Array;

                for(unsigned int k = 0; k < primitive_set->getNumIndices()-2; k=k+inc)
                {
                    osg::Vec3f point_a;
                    osg::Vec3f point_b;
                    osg::Vec3f point_c;

                    // we use set->index because we want to have the good value for all our triangles from the table of all points
                    point_a = osg::Vec3f((* vertices)[primitive_set->index(k)].x(), (* vertices)[primitive_set->index(k)].y(), (* vertices)[primitive_set->index(k)].z()) ;
                    point_b = osg::Vec3f((* vertices)[primitive_set->index(k+1)].x(), (* vertices)[primitive_set->index(k+1)].y(), (* vertices)[primitive_set->index(k+1)].z()) ;
                    point_c = osg::Vec3f((* vertices)[primitive_set->index(k+2)].x(), (* vertices)[primitive_set->index(k+2)].y(), (* vertices)[primitive_set->index(k+2)].z()) ;

                    // keep triangle if at least 1 point is in volume
                    bool contains =
                            (point_a.x()>=XMIN && point_a.x()<=XMAX) &&
                            (point_a.y()>=YMIN && point_a.y()<=YMAX) &&
                            (point_a.z()>=ZMIN && point_a.z()<=ZMAX);
                    if(!contains)
                    {
                        contains = contains || ( (point_b.x()>=XMIN && point_b.x()<=XMAX) &&
                                                 (point_b.y()>=YMIN && point_b.y()<=YMAX) &&
                                                 (point_b.z()>=ZMIN && point_b.z()<=ZMAX));
                    }
                    if(!contains)
                    {
                        contains = contains || ( (point_c.x()>=XMIN && point_c.x()<=XMAX) &&
                                                 (point_c.y()>=YMIN && point_c.y()<=YMAX) &&
                                                 (point_c.z()>=ZMIN && point_c.z()<=ZMAX));
                    }

                    // Warning : in some case, all points are "exterior" of volume but triangle has intersection
                    // must be done plane by plane

                    // Possibility : if not all points are "inside", do real polygon clipping

                    if(contains)
                    {
                        // add triangle
                        outarray->push_back(point_a);
                        outarray->push_back(point_b);
                        outarray->push_back(point_c);
                    }
                }
                if(outarray->size() > 0)
                {

                    osg::ref_ptr<osg::Geode> newgeode = new osg::Geode;
                    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

                    //add triangles
                    geometry->setVertexArray(outarray);
                    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                    osg::Vec4 color(1.0,1.0,1.0,1.0);
                    colors->push_back(color);
                    geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
                    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,outarray->size()));

                    newgeode->addDrawable(geometry);

                    osgUtil::SmoothingVisitor sv;
                    newgeode->accept(sv);

//                    int n = geode.getNumChildren();
//                    osg::StateSet *ss = geode.getOrCreateStateSet();
//                    osg::StateAttribute *attr = ss->getTextureAttribute(0,osg::StateAttribute::TEXTURE);
//                    osg::StateAttribute *attr2 = ss->getTextureAttribute(0,osg::StateAttribute::TEXGEN);
//                    osg::StateAttribute *attr3 = ss->getTextureAttribute(0,osg::StateAttribute::TEXMAT);
//                    if(attr != nullptr)
//                        newgeode->getOrCreateStateSet()->setTextureAttribute(0,attr,osg::StateAttribute::ON);
                    if(ts.size() > 0)
                    {
                        osg::StateSet *ss = newgeode->getOrCreateStateSet();
                        //osg::StateSet::TextureAttributeList ta;
                        TextureSet::iterator it = ts.begin();
                        while(it != ts.end())
                        {
                            osg::Texture *texture = *it;
                            ss->setAttributeAndModes(texture);
                            //ta.push_back(texture);
                            ++it;
                        }
                        //ss->setTextureAttributeList(ta);
                    }
                    (m_clippedNode->asGroup())->addChild(newgeode);
                }
            }
        }
//        else
//        {
//            osg::Drawable *drawable =geode.getDrawable(i);
//            osg::BoundingBox bb = drawable->getBoundingBox();
//            if(bb.intersects(m_boundingBox))
//            {
//                (m_clippedNode->asGroup())->addChild(drawable);
//            }
//        }
    }
}

void ClipModelVisitor::apply(osg::StateSet &stateset)
{
    // search for the existence of any texture object attributes
    for(unsigned int i=0;i<stateset.getTextureAttributeList().size();++i)
    {
        osg::Texture* texture = dynamic_cast<osg::Texture*>(stateset.getTextureAttribute(i,osg::StateAttribute::TEXTURE));
        if (texture)
        {
            m_textureSet.insert(texture);
        }
    }
}

void ClipModelVisitor::setBoundingBox(osg::BoundingBox _boundingBox)
{
    m_boundingBox = _boundingBox;
}

void ClipModelVisitor::setMargin(double margin)
{
    m_margin = margin;
}

