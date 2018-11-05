#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Point>

#include "measurearea.h"

// for area calculation
#include <Eigen/Dense>
#include "Polygon.h"

static std::pair<Eigen::Vector3f, Eigen::Vector3f> bestPlaneFromPoints(std::vector<Eigen::Vector3f> & _points)
{
    // copy coordinates to  matrix in Eigen format
    size_t num_atoms = _points.size();
    Eigen::Matrix< Eigen::Vector3f::Scalar, Eigen::Dynamic, Eigen::Dynamic > coord(3, num_atoms);
    for (size_t i = 0; i < num_atoms; ++i) coord.col(i) = _points[i];

    // calculate centroid
    Eigen::Vector3f centroid(coord.row(0).mean(), coord.row(1).mean(), coord.row(2).mean());

    // subtract centroid
    coord.row(0).array() -= centroid(0); coord.row(1).array() -= centroid(1); coord.row(2).array() -= centroid(2);

    // we only need the left-singular matrix here
    //  http://math.stackexchange.com/questions/99299/best-fitting-plane-given-a-set-of-points
    auto svd = coord.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::Vector3f plane_normal = svd.matrixU().rightCols<1>();
    return std::make_pair(centroid, plane_normal);
}

static void project3DPointsToPlane(std::pair<Eigen::Vector3f, Eigen::Vector3f> &_plane_coeffs, std::vector<Eigen::Vector3f> &_points, std::vector<Eigen::Vector3f> &_projected_points)
{
    Eigen::Vector3f origin=_plane_coeffs.first;
    Eigen::Vector3f normal=_plane_coeffs.second;
    for (unsigned int i=0; i<_points.size(); i++){
        Eigen::Vector3f proj_point = _points[i]-(normal.dot(_points[i]-origin))*normal;
        _projected_points.push_back(proj_point);
    }
}

double static projPointToArea(std::pair<Eigen::Vector3f, Eigen::Vector3f> &_plane_coeffs, std::vector<Eigen::Vector3f> &_projected_points)
{
    Eigen::Vector3f origin=_plane_coeffs.first;
    Eigen::Vector3f normal=_plane_coeffs.second;
    std::vector<double> proj_points_2d_x;
    std::vector<double> proj_points_2d_y;

    Eigen::Vector3f v1 = _projected_points[0]-origin; // take a vector from the plane
    Eigen::Vector3f v2 = v1.cross(normal); // take a vector from the plane

    normal.normalize();
    v1.normalize();
    v2.normalize();

    for (unsigned int i=0; i<_projected_points.size(); i++){
        Eigen::Vector3f P = _projected_points[i]-origin;
        proj_points_2d_x.push_back(P.dot(v1));
        proj_points_2d_y.push_back(P.dot(v2));
    }

    // Construct area Polygon
    basicproc::Polygon selection_polygon;
    selection_polygon.addContour(proj_points_2d_x,proj_points_2d_y);

    return selection_polygon.area();
}

MeasureArea::MeasureArea(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode)
    : MeasureLine(_fieldName, _geode), m_area(0)
{
}

// from JSon to object
void MeasureArea::decode(QJsonObject & _obj)
{
    MeasureLine::decode(_obj);
    QJsonObject p = _obj.value(fieldName()).toObject();
    m_area = p.value("area").toDouble();
}

// encode to JSon
void MeasureArea::encode(QJsonObject & _obj)
{
    MeasureLine::encode(_obj);
    QJsonObject obj = _obj.value(fieldName()).toObject();
    obj.insert("area",QJsonValue(m_area));
    _obj.insert(fieldName(), obj);
}

void MeasureArea::computeLengthAndArea()
{
    computeLength();

    // compute area
    if(m_array.length() < 2)
    {
        m_area =0;
        return;
    }

    std::vector<Eigen::Vector3f> pt_area_data;

    for(int i=1; i<m_array.length(); i++)
    {
        Eigen::Vector3f xyz;

        xyz[0] = m_array[i].x;
        xyz[1] = m_array[i].y;
        xyz[2] = m_array[i].z;

        pt_area_data.push_back(xyz);

    }
    // Fit points to plane
    std::pair<Eigen::Vector3f, Eigen::Vector3f> plane_coeffs = bestPlaneFromPoints(pt_area_data);

    // Project points on plane
    std::vector<Eigen::Vector3f> proj_pt_area_data;
    project3DPointsToPlane(plane_coeffs, pt_area_data, proj_pt_area_data);
    m_area = projPointToArea(plane_coeffs, proj_pt_area_data);
}

void MeasureArea::updateGeode()
{
    m_geode->removeDrawables(0, 2);

    if(m_array.length() == 0)
        return;

    //draw points
    osg::Geometry* shape_point_drawable = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array;

    for(int i=0; i<m_array.length(); i++)
    {
        osg::Vec3d point;
        point[0] = m_array[i].x;
        point[1] = m_array[i].y;
        point[2] = m_array[i].z;
        vertices->push_back(point);
    }

    // pass the created vertex array to the points geometry object.
    shape_point_drawable->setVertexArray(vertices);

    osg::Vec4Array* colors = new osg::Vec4Array;
    // add a yellow color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    osg::Vec4 color(1.0f,1.0f,0.0f,1.0f);
    colors->push_back(color);

    // pass the color array to points geometry, note the binding to tell the geometry
    // that only use one color for the whole object.
    shape_point_drawable->setColorArray(colors, osg::Array::BIND_OVERALL);

    // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
    // parameter passed to the DrawArrays constructor is the Primitive::Mode which
    // in this case is POINTS (which has the same value GL_POINTS), the second
    // parameter is the index position into the vertex array of the first point
    // to draw, and the third parameter is the number of points to draw.
    shape_point_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));
    shape_point_drawable->getOrCreateStateSet()->setAttribute(new osg::Point(4.f), osg::StateAttribute::ON);

    // line
    osg::Geometry* shape_line_drawable = new osg::Geometry();

    // pass the created vertex array to the points geometry object.
    shape_line_drawable->setVertexArray(vertices);

    osg::Vec4Array* colorsl = new osg::Vec4Array;
    // add a red color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    osg::Vec4 colorl(1.0f,0.0f,0.0f,1.0f);
    colorsl->push_back(colorl);

    // pass the color array to points geometry, note the binding to tell the geometry
    // that only use one color for the whole object.
    shape_line_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);

    // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
    // parameter passed to the DrawArrays constructor is the Primitive::Mode which
    // in this case is POINTS (which has the same value GL_POINTS), the second
    // parameter is the index position into the vertex array of the first point
    // to draw, and the third parameter is the number of points to draw.
    shape_line_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,vertices->size()));

    m_geode->addDrawable(shape_line_drawable);
    m_geode->addDrawable(shape_point_drawable);
}


void MeasureArea::save()
{
    m_arraySave = m_array;
}

void MeasureArea::restore()
{
    m_array = m_arraySave;
    computeLengthAndArea();
    updateGeode();
}

void MeasureArea::cancel()
{
    m_arraySave.clear();
}
