#include "interest_point_tool.h"
#include <math.h>
#include "tool_handler.h"

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

QString InterestPointTool::interestPointCoordinates()
{
    double x=0;
    double y=0;
    double z=0;

    x = m_measurement_pt->at(0)[0];
    y = m_measurement_pt->at(0)[1];
    z = m_measurement_pt->at(0)[2];

    QString xCoordinates;
    QString yCoordinates;
    QString zCoordinates;

    xCoordinates = QString::number(x);
    yCoordinates = QString::number(y);
    zCoordinates = QString::number(z);

    m_coordinates = xCoordinates + " , " + yCoordinates + " , " + zCoordinates;

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

        meas_list << points_object;
    }

    _root_obj.insert("interest_points",meas_list);
}

void InterestPointTool::decodeJSON(QJsonObject &_root_obj)
{

}

void InterestPointTool::endMeasurement()
{
    // Compute lineLength and affect it in history map
    if(m_measurement_pt)
        interestPointCoordinates();

    // Call parent method
    MeasurementTool::endMeasurement();
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
              m_tool_handler->forceGeodeUpdate();
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

