#include "surface_measurement_tool.h"
#include <math.h>
#include "Polygon.h"
#include "tool_handler.h"

SurfaceMeasurementTool::SurfaceMeasurementTool(ToolHandler *_tool_handler):MeasurementTool(_tool_handler),m_area(-10)
{
    m_meas_type = SURFACE_MEASUREMENT_STATE;
}

SurfaceMeasurementTool::~SurfaceMeasurementTool()
{

}



void SurfaceMeasurementTool::draw()
{

    if(m_measurement_pt->size() == 1)
    {
        m_last_meas_idx++;

        // point
        QString point_name = QString("measurement_%1point_%2").arg(m_last_meas_idx).arg(m_measurement_pt->size());
        osg::Vec4 color(1.0f,1.0f,0.0f,1.0f);
        drawPoint(m_measurement_pt->back(),color,point_name);

    }

    else if(m_measurement_pt->size() >= 2)
    {


        // points
        QString point_name = QString("measurement_%1point_%2").arg(m_last_meas_idx).arg(m_measurement_pt->size());
        osg::Vec4 color(1.0f,1.0f,0.0f,1.0f);
        drawPoint(m_measurement_pt->back(),color,point_name);


        QString line_name = QString("measurement_%1line_%2").arg(m_last_meas_idx).arg(m_measurement_pt->size()-1);
        drawJunctionLineWithLastPoint(line_name);

    }


}

void SurfaceMeasurementTool::cancelMeasurement()
{
    if(m_measurement_pt){
        for(unsigned int i=1; i<=m_measurement_pt->size(); ++i)
        {
            QString point_key = QString("measurement_%1point_%2").arg(m_last_meas_idx).arg(i);
            m_measurement_geode->removeDrawable(m_geo_drawable_map[point_key]);
            m_geo_drawable_map.remove(point_key);
        }

        for(unsigned int j=1; j<=m_measurement_pt->size()-1; ++j)
        {
            QString line_key = QString("measurement_%1line_%2").arg(m_last_meas_idx).arg(j);
            m_measurement_geode->removeDrawable(m_geo_drawable_map[line_key]);
            m_geo_drawable_map.remove(line_key);
        }

        m_last_meas_idx--;
        m_measurement_pt = NULL;
    }
}


void SurfaceMeasurementTool::removeLastMeasurement()
{
    removeMeasurement(m_last_meas_idx);
}



void SurfaceMeasurementTool::removeMeasurement(int _meas_index)
{

    for(unsigned int i=1; i<=m_measurements_pt_qmap[_meas_index]->size(); ++i)
    {
        QString point_key = QString("measurement_%1point_%2").arg(_meas_index).arg(i);
        m_measurement_geode->removeDrawable(m_geo_drawable_map[point_key]);
        m_geo_drawable_map.remove(point_key);
    }

    for(unsigned int j=1; j<=m_measurements_pt_qmap[_meas_index]->size()-1; ++j)
    {
        QString line_key = QString("measurement_%1line_%2").arg(_meas_index).arg(j);
        m_measurement_geode->removeDrawable(m_geo_drawable_map[line_key]);
        m_geo_drawable_map.remove(line_key);
    }

    m_measurements_pt_qmap.remove(_meas_index);

}



int SurfaceMeasurementTool::getMeasurementCounter() const
{
    return m_last_meas_idx;
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

QString SurfaceMeasurementTool::getTextFormattedResult()
{
    return (QString::number(m_area,'f',3) + " m²");
}

void SurfaceMeasurementTool::encodeToJSON(QJsonObject & _root_obj)
{
    QJsonArray meas_list;

    for( QMap<int, osg::ref_ptr<osg::Vec3dArray>>::iterator it = m_measurements_pt_qmap.begin(); it != m_measurements_pt_qmap.end(); it++ )
    {
        QJsonObject points_object;
        QJsonArray points_vector;

        osg::ref_ptr<osg::Vec3dArray> meas = it.value();

        for (unsigned int i=0; i<meas->size(); i++){
            QJsonArray xyz;
            xyz << (double)meas->at(i)[0] << (double)meas->at(i)[1] << (double)meas->at(i)[2];
            points_vector << xyz;
        }

        points_object["name"]=m_measurements_name_qmap[it.key()];
        points_object["points"]=points_vector;
        points_object["area"]=m_measurements_area[it.key()];

        meas_list << points_object;
    }

    _root_obj.insert("area_measurements",meas_list);
}

void SurfaceMeasurementTool::decodeJSON(QJsonObject &_root_obj)
{
    QJsonArray meas_list;

    meas_list = _root_obj["area_measurements"].toArray();

    if(meas_list.isEmpty())
        return;

    // Cancel current measurment (in case it is needed)
    cancelMeasurement();

    for (int i=0; i<meas_list.size(); i++)
    {
        QJsonObject points_object = meas_list.at(i).toObject();

        QString meas_name = points_object["name"].toString();
        //double meas_are = points_object["area"].toDouble();
        QJsonArray points_vector = points_object["points"].toArray();

        for (int j=0; j<points_vector.size(); j++)
        {
            QJsonArray xyz_json=points_vector.at(j).toArray();
            osg::Vec3d xyz_osg(xyz_json.at(0).toDouble(),xyz_json.at(1).toDouble(),xyz_json.at(2).toDouble());
            pushNewPoint(xyz_osg);
        }

        //m_area = meas_area;
        setCurrentMeasName(meas_name);
        endMeasurement(true);

    }
}

void SurfaceMeasurementTool::onMousePress(Qt::MouseButton _button, int _x, int _y)
{
    switch (_button) {
    case Qt::LeftButton:
    {
        osg::Vec3d inter_point;
        bool inter_exists;
        m_tool_handler->getIntersectionPoint(_x, _y, inter_point, inter_exists);
        if(inter_exists){
            pushNewPoint(inter_point);
        }
    }
        break;
    case Qt::MiddleButton:
    {
        closeLoop();
        endMeasurement();
    }
        break;
    case Qt::RightButton:
        closeLoop();
        endMeasurement();
        break;
    default:
        break;
    }
}


void SurfaceMeasurementTool::endMeasurement(bool _meas_info_is_set)
{
    // Compute lineLength and affect it in history map
    if(m_measurement_pt)
        m_measurements_area[m_last_meas_idx] = getArea();

    // Call parent method
    MeasurementTool::endMeasurement(_meas_info_is_set);
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
