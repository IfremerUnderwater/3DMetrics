#include "line_measurement_tool.h"
#include <math.h>
#include "tool_handler.h"


LineMeasurementTool::LineMeasurementTool(ToolHandler *_tool_handler):MeasurementTool(_tool_handler)
{
    m_meas_type = LINE_MEASUREMENT_STATE;
}

LineMeasurementTool::~LineMeasurementTool()
{

}


void LineMeasurementTool::draw()
{


    if(m_measurement_pt->size() == 1)
    {
        m_last_meas_idx++;

        // point
        QString point_name = QString("measurement_%1point_%2").arg(m_last_meas_idx).arg(m_measurement_pt->size());
        osg::Vec4 color(1.0f,0.0f,0.0f,1.0f);
        drawPoint(m_measurement_pt->back(),color,point_name);


    }
    else if(m_measurement_pt->size() >= 2)
    {

        // points
        QString point_name = QString("measurement_%1point_%2").arg(m_last_meas_idx).arg(m_measurement_pt->size());

        osg::Vec4 color(1.0f,0.0f,0.0f,1.0f);
        drawPoint(m_measurement_pt->back(),color,point_name);

        // lines

        QString line_name = QString("measurement_%1line_%2").arg(m_last_meas_idx).arg(m_measurement_pt->size()-1);
        drawJunctionLineWithLastPoint(line_name);

    }


    lineLength();
}

void LineMeasurementTool::cancelMeasurement()
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

void LineMeasurementTool::endMeasurement(bool _meas_info_is_set)
{
    // Compute lineLength and affect it in history map
    if(m_measurement_pt)
        m_measurements_length[m_last_meas_idx] = lineLength();

    // Call parent method
    MeasurementTool::endMeasurement(_meas_info_is_set);
}



void LineMeasurementTool::removeLastMeasurement()
{
    removeMeasurement(m_last_meas_idx);
}



void LineMeasurementTool::removeMeasurement(int _meas_index)
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
    m_measurements_length.remove(_meas_index);

}


double LineMeasurementTool::lineLength()
{
    if(m_measurement_pt->size() >= 2)
    {

        double x=0, y=0, z=0;
        m_norm=0;

        for(unsigned int i=1; i<m_measurement_pt->size(); ++i)
        {
            x = fabs(m_measurement_pt->at(i)[0] - m_measurement_pt->at(i-1)[0]);
            y = fabs(m_measurement_pt->at(i)[1] - m_measurement_pt->at(i-1)[1]);
            z = fabs(m_measurement_pt->at(i)[2] - m_measurement_pt->at(i-1)[2]);

            m_norm += sqrt(x*x + y*y + z*z);
        }

        return m_norm;

    }else
    {
        m_norm = 0.0;
        return 0.0;
    }
}


int LineMeasurementTool::getMeasurementCounter() const
{
    return m_last_meas_idx;
}

QString LineMeasurementTool::getTextFormattedResult()
{
    return (QString::number(m_norm,'f',3) + " m");
}

void LineMeasurementTool::encodeToJSON(QJsonObject & _root_obj)
{

    QJsonArray meas_list;

    for( QMap<int, osg::ref_ptr<osg::Vec3dArray>>::iterator it = m_measurements_pt_qmap.begin(); it != m_measurements_pt_qmap.end(); it++ )
    {
        QJsonObject points_object;
        QJsonArray points_vector;

        osg::ref_ptr<osg::Vec3dArray> meas = it.value();

        for (unsigned int i=0; i<meas->size(); i++)
        {
            QJsonArray xyz;
            xyz << (double)meas->at(i)[0] << (double)meas->at(i)[1] << (double)meas->at(i)[2];
            points_vector << xyz;
        }

        points_object["name"]=m_measurements_name_qmap[it.key()];
        points_object["points"]=points_vector;
        points_object["length"]=m_measurements_length[it.key()];
        points_object["comment"]=m_measurements_comment_qmap[it.key()];
        points_object["temp"]=m_measurements_temp_qmap[it.key()];
        points_object["category"]=m_measurements_category_qmap[it.key()];

        meas_list << points_object;
    }

    _root_obj.insert("line_measurements",meas_list);

}

void LineMeasurementTool::decodeJSON(QJsonObject &_root_obj)
{
    QJsonArray meas_list;

    meas_list = _root_obj["line_measurements"].toArray();

    if(meas_list.isEmpty())
        return;

    // Cancel current measurment (in case it is needed)
    cancelMeasurement();

    for (int i=0; i<meas_list.size(); i++)
    {
        QJsonObject points_object = meas_list.at(i).toObject();

        QString meas_name = points_object["name"].toString();
        QString meas_comment = points_object["comment"].toString();
        QString meas_temp = points_object["temp"].toString();
        QString meas_category = points_object["category"].toString();

        //double meas_length = points_object["length"].toDouble();
        QJsonArray points_vector = points_object["points"].toArray();

        for (int j=0; j<points_vector.size(); j++)
        {
            QJsonArray xyz_json=points_vector.at(j).toArray();
            osg::Vec3d xyz_osg(xyz_json.at(0).toDouble(),xyz_json.at(1).toDouble(),xyz_json.at(2).toDouble());
            pushNewPoint(xyz_osg);
        }

        //m_norm = meas_length;
        setCurrentMeasInfo(meas_name, meas_comment, meas_temp, meas_category);
        endMeasurement(true);

    }

}

void LineMeasurementTool::onMousePress(Qt::MouseButton _button, int _x, int _y)
{
    switch (_button) {
    case Qt::LeftButton:
    {
        osg::Vec3d inter_point;
        bool inter_exists;
        m_tool_handler->getIntersectionPoint(_x, _y, inter_point, inter_exists);
        if(inter_exists){
            pushNewPoint(inter_point);
            m_tool_handler->forceGeodeUpdate();
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
        endMeasurement();
        break;
    default:
        break;
    }
}






