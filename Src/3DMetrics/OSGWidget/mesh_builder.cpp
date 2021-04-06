#include "mesh_builder.h"

#include "geometry_type_count_visitor.h"

#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osgUtil/DelaunayTriangulator>
#include <osgDB/WriteFile>

MeshBuilder::MeshBuilder(osg::ref_ptr<osg::Node> _node) : m_node(_node)
{
    // on mean triangle size (squares edge sizes)
    m_threshold = 1000000000.0f;

    // on mean absolute delta
    m_thresholdx = 1000000000.0f;
    m_thresholdy = 1000000000.0f;
    m_thresholdz = 1000000000.0f;
}

bool MeshBuilder::hasPointsAndNotMesh()
{
    osg::ref_ptr<osg::MatrixTransform> matrix = dynamic_cast<osg::MatrixTransform*>(m_node.get());
    osg::ref_ptr<osg::Node> root = matrix->getChild(0);
    GeometryTypeCountVisitor geomcount;
    matrix->accept(geomcount);

    if(geomcount.getNbTriangles() == 0 && geomcount.getNbPoints() > 0 && root->asGeode() != nullptr)
        return true;

    return false;
}

bool MeshBuilder::Triangulate()
{
    osg::ref_ptr<osg::MatrixTransform> matrix = dynamic_cast<osg::MatrixTransform*>(m_node.get());
    osg::ref_ptr<osg::Node> root = matrix->getChild(0);
    GeometryTypeCountVisitor geomcount;
    matrix->accept(geomcount);

    bool returnstatus = false;

    // Delaunaytriangulation for models with only points
    if(geomcount.getNbTriangles() == 0 && geomcount.getNbPoints() > 0 && root->asGeode() != nullptr)
    {

        osg::ref_ptr<osgUtil::DelaunayTriangulator> dt = new osgUtil::DelaunayTriangulator;
        //osg::ref_ptr<osg3DMETRICS::DelaunayTriangulatorNosort> dt = new osg3DMETRICS::DelaunayTriangulatorNosort;

        osg::Geode *geode = root->asGeode();
        unsigned int num_drawables = geode->getNumDrawables();
        for( unsigned int i = 0; i < num_drawables; i++ )
        {
            // Use 'asGeometry' as its supposed to be faster than a dynamic_cast
            // every little saving counts
            osg::Geometry *current_geometry = geode->getDrawable(i)->asGeometry();

            // Only process if the drawable is geometry
            if ( current_geometry )
            {
                // get the list of different geometry mode which were created
                osg::Geometry::PrimitiveSetList primitive_list = current_geometry->getPrimitiveSetList();

                for(unsigned int j = 0; j < primitive_list.size(); j++)
                {
                    osg::PrimitiveSet *primitive_set = primitive_list[j];

                    if(primitive_set->getMode() == osg::PrimitiveSet::POINTS)
                    {
                        osg::Array *array = current_geometry->getVertexArray();
                        osg::Vec3Array *v = new osg::Vec3Array;
                        v->resize(((osg::Vec3Array*)array)->size());
                        std::copy( ((osg::Vec3Array*)array)->begin(), ((osg::Vec3Array*)array)->end(), v->begin() );

                        dt->setInputPointArray(v);
                        dt->setOutputNormalArray( new osg::Vec3Array );
                        dt->triangulate();

                        // rebuild in order not to use depracated osg::Geometry::BIND_PER_PRIMITIVE
                        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
                        osg::Vec3Array* vertexes = dt->getInputPointArray();

                        osg::Vec3Array* normals = dt->getOutputNormalArray();
                        osg::Vec3Array::iterator nitr = normals->begin();

                        osg::DrawElementsUInt *indexes = dt->getTriangles();
                        osg::DrawElementsUInt::iterator iitr = indexes->begin();

                        if(indexes == nullptr || indexes->size() == 0)
                            continue;

                        // calculate mean triangle edge size
                        // sum of dx*dx+dy*dy+dz*dz
                        double sum = 0;
                        double sumdx = 0;
                        double sumdy = 0;
                        double sumdz = 0;
                        while(iitr != indexes->end())
                        {
                            osg::Vec3 a = (*vertexes)[*iitr];
                            ++iitr;
                            osg::Vec3 b = (*vertexes)[*iitr];
                            ++iitr;
                            osg::Vec3 c = (*vertexes)[*iitr];
                            ++iitr;

                            // ab
                            sum += (a.x()-b.x())*(a.x()-b.x());
                            sum += (a.y()-b.y())*(a.y()-b.y());
                            sum += (a.z()-b.z())*(a.z()-b.z());
                            sumdx += fabs(a.x() - b.x());
                            sumdy += fabs(a.y() - b.y());
                            sumdz += fabs(a.z() - b.z());

                            // ac
                            sum += (a.x()-c.x())*(a.x()-c.x());
                            sum += (a.y()-c.y())*(a.y()-c.y());
                            sum += (a.z()-c.z())*(a.z()-c.z());
                            sumdx += fabs(a.x() - c.x());
                            sumdy += fabs(a.y() - c.y());
                            sumdz += fabs(a.z() - c.z());

                            // bc
                            sum += (b.x()-c.x())*(b.x()-c.x());
                            sum += (b.y()-c.y())*(b.y()-c.y());
                            sum += (b.z()-c.z())*(b.z()-c.z());
                            sumdx += fabs(b.x() - c.x());
                            sumdy += fabs(b.y() - c.y());
                            sumdz += fabs(b.z() - c.z());

                        }
                        double mean = sum / indexes->size();
                        double meandx = sumdx / indexes->size();
                        double meandy = sumdy / indexes->size();
                        double meandz = sumdz / indexes->size();

                        float threshold = m_threshold * mean; // warning : squares !
                        float thresholdx = m_thresholdx * meandx;
                        float thresholdy = m_thresholdy * meandy;
                        float thresholdz = m_thresholdz * meandz;

                        osg::ref_ptr<osg::Vec3Array> outPoints = new osg::Vec3Array;
                        osg::ref_ptr<osg::Vec3Array> outNormals = new osg::Vec3Array;

                        nitr = normals->begin();
                        iitr = indexes->begin();
                        while(nitr != normals->end())
                        {
                            osg::Vec3 a = (*vertexes)[*iitr];
                            ++iitr;
                            osg::Vec3 b = (*vertexes)[*iitr];
                            ++iitr;
                            osg::Vec3 c = (*vertexes)[*iitr];
                            ++iitr;

                            // triangles with normals to bottom
                            osg::Vec3 n = *nitr;
                            ++nitr;

                            // ab
                            if((a.x()-b.x())*(a.x()-b.x()) > threshold)
                                continue;
                            if((a.y()-b.y())*(a.y()-b.y()) > threshold)
                                continue;
                            if((a.z()-b.z())*(a.z()-b.z()) > threshold)
                                continue;
                            // ac
                            if((a.x()-c.x())*(a.x()-c.x()) > threshold)
                                continue;
                            if((a.y()-c.y())*(a.y()-c.y()) > threshold)
                                continue;
                            if((a.z()-c.z())*(a.z()-c.z()) > threshold)
                                continue;

                            // bc
                            if((b.x()-c.x())*(b.x()-c.x()) > threshold)
                                continue;
                            if((b.y()-c.y())*(b.y()-c.y()) > threshold)
                                continue;
                            if((b.z()-c.z())*(b.z()-c.z()) > threshold)
                                continue;

                            // dx threshold
                            if(fabs(a.x() - b.x()) > thresholdx)
                                continue;
                            if(fabs(a.x() - c.x()) > thresholdx)
                                continue;
                            if(fabs(b.x() - c.x()) > thresholdx)
                                continue;

                            // dy threshold
                            if(fabs(a.y() - b.y()) > thresholdy)
                                continue;
                            if(fabs(a.y() - c.y()) > thresholdy)
                                continue;
                            if(fabs(b.y() - c.y()) > thresholdy)
                                continue;

                            // dz threshold
                            if(fabs(a.z() - b.z()) > thresholdz)
                                continue;
                            if(fabs(a.z() - c.z()) > thresholdz)
                                continue;
                            if(fabs(b.z() - c.z()) > thresholdz)
                                continue;

                            outPoints->push_back(a);
                            outPoints->push_back(b);
                            outPoints->push_back(c);

                            if(n.z() < 0)
                            {
                                n.x() = -n.x();
                                n.y() = -n.y();
                                n.z() = -n.z();
                            }
                            outNormals->push_back(n);
                            outNormals->push_back(n);
                            outNormals->push_back(n);
                        }
                        geometry->setVertexArray( outPoints );
                        geometry->setNormalArray( outNormals );
                        geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

                        bool res = geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,outPoints->size()));

                        m_generatedGeometry = geometry;

                        osg::Vec4Array *color = new osg::Vec4Array;
                        osg::Vec4f c(0.5f, 0.5f,0.5f,0.5f);
                        color->push_back(c);
                        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

                        osg::StateSet* stateSet = geometry->getOrCreateStateSet();
                        stateSet->setMode( GL_BLEND, osg::StateAttribute::ON);
                        stateSet->setRenderBinDetails(2, "RenderBin");

                        //
                        osg::StateSet* stateSetpts = current_geometry->getOrCreateStateSet();
                        stateSetpts->setMode( GL_BLEND, osg::StateAttribute::ON);
                        stateSetpts->setRenderBinDetails(1, "RenderBin");
                        //stateSetpts->addUniform( new osg::Uniform( "hasmesh", false));

                        // material
                        // Add the possibility of modifying the transparence
                        osg::Material* material = material = new osg::Material;

                        //material->setDiffuse( osg::Material::FRONT, osg::Vec4( 0.5f, 0.5f, 0.5f, 1.f ) );
                        //material->setSpecular( osg::Material::FRONT, osg::Vec4( 1.f, 1.f, 1.f, 0.f ) );
                        material->setShininess( osg::Material::FRONT, 96.f );
                        material->setEmission( osg::Material::FRONT, osg::Vec4( 0.4f, 0.4f, 0.4f, 0.f ) );

                        // Put the 3D model 50% opaque
                        stateSet->setAttributeAndModes ( material, osg::StateAttribute::ON );
                        material->setAlpha(osg::Material::FRONT, 0.5f );
                        //stateSet->addUniform( new osg::Uniform( "hasmesh", true));

                        osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::ONE_MINUS_SRC_ALPHA,osg::BlendFunc::SRC_ALPHA );
                        stateSet->setAttributeAndModes(bf);

                        geode->addDrawable( geometry.get() );

                        returnstatus = true;
                    }
                }
            }
        }
    }
    return returnstatus;
}

bool MeshBuilder::saveGeneratedMesh(std::string _fileName)
{
    osg::ref_ptr<osg::Geode> geodesave = new osg::Geode;
    geodesave->addDrawable( m_generatedGeometry.get() );
    bool status = osgDB::writeNodeFile(*geodesave,
                                       _fileName,
                                       new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib"));
    return status;
}

void MeshBuilder::setThreshold(float threshold)
{
    m_threshold = threshold;
}

void MeshBuilder::setThresholdx(float thresholdx)
{
    m_thresholdx = thresholdx;
}

void MeshBuilder::setThresholdy(float thresholdy)
{
    m_thresholdy = thresholdy;
}

void MeshBuilder::setThresholdz(float thresholdz)
{
    m_thresholdz = thresholdz;
}
