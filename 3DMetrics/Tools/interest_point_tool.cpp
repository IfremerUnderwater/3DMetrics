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

    if(m_measurement_pt->size() >= 1)
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
    for(unsigned int i=1; i<=m_measurement_pt->size(); ++i)
    {
        QString point_key = QString("measurement_%1point_%2").arg(m_last_meas_idx).arg(i);
        m_measurement_geode->removeDrawable(m_geo_drawable_map[point_key]);
        m_geo_drawable_map.remove(point_key);
    }

    m_last_meas_idx--;
    m_measurement_pt = NULL;

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

    m_last_meas_idx--;

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

}


