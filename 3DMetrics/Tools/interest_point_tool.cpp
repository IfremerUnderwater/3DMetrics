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
        m_measurement_counter++;

        // point
        QString point_name = QString("measurement_%1").arg(m_measurement_counter);

        osg::Vec4 color(0.0f,0.0f,1.0f,1.0f);
        drawPoint(m_measurement_pt->back(),color,point_name);

    }

    m_meas_points_number[m_measurement_counter] = m_measurement_pt->size();

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


void InterestPointTool::removeLastMeasurement()
{
    removeMeasurement(m_measurement_counter);
}


void InterestPointTool::removeMeasurement(int _meas_index)
{
    QString point_number = QString("measurement_%1").arg(_meas_index);

    m_measurement_geode->removeDrawable(m_geo_drawable_map[point_number]);
    m_geo_drawable_map.remove(point_number);

    m_measurements_pt_qmap.remove(_meas_index);

    m_measurement_counter--;

}


int InterestPointTool::getMeasurementCounter() const
{
    return m_measurement_counter;
}


void InterestPointTool::hideShowMeasurement(int _meas_index, bool _visible)
{

    if (_visible)
    {
        QString point_number = QString("measurement_%1").arg(_meas_index);

        if(!m_measurement_geode->containsDrawable(m_geo_drawable_map[point_number]))
        {
            m_measurement_geode->addDrawable(m_geo_drawable_map[point_number]);
            qDebug() << "Add measur";
        }
    }
    else
    {
        QString point_number = QString("measurement_%1").arg(_meas_index);

        if(m_measurement_geode->containsDrawable(m_geo_drawable_map[point_number]))
        {
            m_measurement_geode->removeDrawable(m_geo_drawable_map[point_number]);
            qDebug() << "Remove measur";
        }
    }
}



void InterestPointTool::closeLoop()
{
    // this method is not used in this class
}



void InterestPointTool::resetInterestPointData()
{
    m_coordinates.clear();
}

