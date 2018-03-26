#include "interest_point_tool.h"
#include <math.h>
#include "tool_handler.h"
#include <GeographicLib/LocalCartesian.hpp>

InterestPointTool::InterestPointTool(ToolHandler *_tool_handler):MeasurementTool(_tool_handler)
{
    m_meas_type = INTEREST_POINT_STATE;
}


InterestPointTool::~InterestPointTool()
{

}


void InterestPointTool::draw()
{

    if(m_measurement_pt)
    {
        m_last_meas_idx++;

        // point
        QString point_name = QString("measurement_%1").arg(m_last_meas_idx);

        osg::Vec4 color(0.0f,0.0f,1.0f,1.0f);
        drawPoint(m_measurement_pt->back(),color,point_name);

    }

}

QString InterestPointTool::interestPointCoordinatesToText()
{
    double x=0;
    double y=0;
    double z=0;

    x = m_measurement_pt->at(0)[0];
    y = m_measurement_pt->at(0)[1];
    z = m_measurement_pt->at(0)[2];

    double lat, lon, depth;

    // transform to lat/lon
    QPointF _ref_lat_lon; double _ref_depth;
    m_tool_handler->getGeoOrigin(_ref_lat_lon, _ref_depth);
    GeographicLib::LocalCartesian ltp_proj;
    ltp_proj.Reset(_ref_lat_lon.x(), _ref_lat_lon.y(),_ref_depth);
    ltp_proj.Reverse(x, y, z, lat, lon, depth);

    QString lat_coordinates, lon_coordinates, depth_coordinates;
    lat_coordinates = QString::number(lat,'g',13);
    lon_coordinates = QString::number(lon,'g',13);
    depth_coordinates = QString::number(depth,'g',5);

    m_coordinates = lat_coordinates + " , " + lon_coordinates + " , " + depth_coordinates;

    return m_coordinates;
}


void InterestPointTool::cancelMeasurement()
{
    if(m_measurement_pt){
        for(unsigned int i=1; i<=m_measurement_pt->size(); ++i)
        {
            QString point_key = QString("measurement_%1point_%2").arg(m_last_meas_idx).arg(i);
            m_measurement_geode->removeDrawable(m_geo_drawable_map[point_key]);
            m_geo_drawable_map.remove(point_key);
        }

        m_last_meas_idx--;
        m_measurement_pt = NULL;
    }

}


void InterestPointTool::removeLastMeasurement()
{
    removeMeasurement(m_last_meas_idx);
}


void InterestPointTool::removeMeasurement(int _meas_index)
{
    QString point_key = QString("measurement_%1").arg(_meas_index);

    m_measurement_geode->removeDrawable(m_geo_drawable_map[point_key]);
    m_geo_drawable_map.remove(point_key);

    m_measurements_pt_qmap.remove(_meas_index);

}


int InterestPointTool::getMeasurementCounter() const
{
    return m_last_meas_idx;
}


void InterestPointTool::hideShowMeasurement(int _meas_index, bool _visible)
{

    if (_visible)
    {
        QString point_key = QString("measurement_%1").arg(_meas_index);

        if(!m_measurement_geode->containsDrawable(m_geo_drawable_map[point_key]))
        {
            m_measurement_geode->addDrawable(m_geo_drawable_map[point_key]);
        }
    }
    else
    {
        QString point_key = QString("measurement_%1").arg(_meas_index);

        if(m_measurement_geode->containsDrawable(m_geo_drawable_map[point_key]))
        {
            m_measurement_geode->removeDrawable(m_geo_drawable_map[point_key]);
        }
    }
}



void InterestPointTool::closeLoop()
{
    // this method is not used in this class
}

QString InterestPointTool::getTextFormattedResult()
{
    return m_coordinates;
}

void InterestPointTool::encodeToJSON(QJsonObject & _root_obj)
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
        points_object["comment"]=m_measurements_comment_qmap[it.key()];
        points_object["temp"]=m_measurements_temp_qmap[it.key()];
        points_object["category"]=m_measurements_category_qmap[it.key()];

        meas_list << points_object;
    }

    _root_obj.insert("interest_points",meas_list);
}

void InterestPointTool::decodeJSON(QJsonObject &_root_obj)
{
    QJsonArray meas_list;

    meas_list = _root_obj["interest_points"].toArray();

    if(meas_list.isEmpty())
        return;

    // Cancel current measurment (in case it is needed)
    cancelMeasurement();

    for (int i=0; i<meas_list.size(); i++)
    {
        QJsonObject points_object = meas_list.at(i).toObject();

        QString meas_name = points_object["name"].toString();
        QJsonArray points_vector = points_object["points"].toArray();
        QString meas_comment = points_object["comment"].toString();
        QString meas_temp = points_object["temp"].toString();
        QString meas_category = points_object["category"].toString();

        for (int j=0; j<points_vector.size(); j++)
        {
            QJsonArray xyz_json=points_vector.at(j).toArray();
            osg::Vec3d xyz_osg(xyz_json.at(0).toDouble(),xyz_json.at(1).toDouble(),xyz_json.at(2).toDouble());
            pushNewPoint(xyz_osg);
        }

        setCurrentMeasInfo(meas_name, meas_comment, meas_temp, meas_category);
        endMeasurement(true);

    }
}

void InterestPointTool::endMeasurement(bool _meas_info_is_set)
{
    // Compute lineLength and affect it in history map
    if(m_measurement_pt)
        interestPointCoordinatesToText();

    // Call parent method
    MeasurementTool::endMeasurement(_meas_info_is_set);
}

void InterestPointTool::onMousePress(Qt::MouseButton _button, int _x, int _y)
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
        endMeasurement();
    }
        break;
    case Qt::MiddleButton:
        break;
    case Qt::RightButton:
        break;
    default:
        break;
    }
}

