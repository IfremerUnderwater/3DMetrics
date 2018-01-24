#include "surface_measurement_tool.h"
#include <math.h>
#include "Polygon.h"

SurfaceMeasurementTool::SurfaceMeasurementTool():MeasurementTool(),m_area(-10)
{

}

SurfaceMeasurementTool::~SurfaceMeasurementTool()
{

}



void SurfaceMeasurementTool::draw()
{
    m_measur_type = "Surface measurement";

    if(m_measurement_pt->size() == 1)
    {
        m_measurement_counter++;

        // point
        QString point_name = QString("measurement_%1point_%2").arg(m_measurement_counter).arg(m_measurement_pt->size());
        osg::Vec4 color(1.0f,1.0f,0.0f,1.0f);
        drawPoint(m_measurement_pt->back(),color,point_name);

    }

    else if(m_measurement_pt->size() >= 2)
    {


        // points
        QString point_name = QString("measurement_%1point_%2").arg(m_measurement_counter).arg(m_measurement_pt->size());
        osg::Vec4 color(1.0f,1.0f,0.0f,1.0f);
        drawPoint(m_measurement_pt->back(),color,point_name);


        // lines
        m_lines_counter++;

        QString line_name = QString("measurement_%1line_%2").arg(m_measurement_counter).arg(m_measurement_pt->size()-1);
        drawJunctionLineWithLastPoint(line_name);

    }

    m_meas_points_number[m_measurement_counter] = m_measurement_pt->size();

    m_meas_lines_number[m_measurement_counter] = m_lines_counter;

}



void SurfaceMeasurementTool::removeLastMeasurement()
{
    removeMeasurement(m_measurement_counter);
}



void SurfaceMeasurementTool::removeMeasurement(int _meas_index)
{

    for(int i=1; i<=m_meas_points_number[_meas_index]; ++i)
    {
        QString point_number = QString("measurement_%1point_%2").arg(_meas_index).arg(i);
        m_measurement_geode->removeDrawable(m_geo_drawable_map[point_number]);
        m_geo_drawable_map.remove(point_number);
    }

    for(int j=1; j<=m_meas_points_number[_meas_index]; ++j)
    {
        QString line_number = QString("measurement_%1line_%2").arg(_meas_index).arg(j);
        m_measurement_geode->removeDrawable(m_geo_drawable_map[line_number]);
        m_geo_drawable_map.remove(line_number);
    }

    m_measurements_pt_qmap.remove(_meas_index);

    m_measurement_counter--;

}


QString SurfaceMeasurementTool::getTypeOfMeasur()
{
    return m_measur_type;
}


int SurfaceMeasurementTool::getMeasurementCounter() const
{
    return m_measurement_counter;
}


double SurfaceMeasurementTool::getArea()
{
    std::vector<Eigen::Vector3f> pt_area_data;

    for(unsigned int i=0; i<m_measurement_pt->size(); ++i)
    {

        Eigen::Vector3f xyz;

        xyz[0] = m_measurement_pt->at(i)[0];
        xyz[1] = m_measurement_pt->at(i)[1];
        xyz[2] = m_measurement_pt->at(i)[2];

        pt_area_data.push_back(xyz);

    }
    // Fit points to plane
    std::pair<Eigen::Vector3f, Eigen::Vector3f> plane_coeffs = bestPlaneFromPoints(pt_area_data);

    // Project points on plane
    std::vector<Eigen::Vector3f> proj_pt_area_data;
    project3DPointsToPlane(plane_coeffs, pt_area_data, proj_pt_area_data);
    m_area = projPointToArea(plane_coeffs, proj_pt_area_data);

    return m_area;
}


void SurfaceMeasurementTool::resetSurfaceData()
{
    m_lastNorm = 0;
    m_area = 0;
}

//template<class Vector3>
std::pair<Eigen::Vector3f, Eigen::Vector3f> SurfaceMeasurementTool::bestPlaneFromPoints(std::vector<Eigen::Vector3f> & _points)
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

void SurfaceMeasurementTool::project3DPointsToPlane(std::pair<Eigen::Vector3f, Eigen::Vector3f> &_plane_coeffs, std::vector<Eigen::Vector3f> &_points, std::vector<Eigen::Vector3f> &_projected_points){
    Eigen::Vector3f origin=_plane_coeffs.first;
    Eigen::Vector3f normal=_plane_coeffs.second;
    for (unsigned int i=0; i<_points.size(); i++){
        Eigen::Vector3f proj_point = _points[i]-(normal.dot(_points[i]-origin))*normal;
        _projected_points.push_back(proj_point);
    }
}

double SurfaceMeasurementTool::projPointToArea(std::pair<Eigen::Vector3f, Eigen::Vector3f> &_plane_coeffs, std::vector<Eigen::Vector3f> &_projected_points)
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
