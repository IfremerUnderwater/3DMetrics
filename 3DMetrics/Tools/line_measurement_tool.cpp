#include "line_measurement_tool.h"
#include <math.h>

LineMeasurementTool::LineMeasurementTool(ToolHandler *_tool_handler):MeasurementTool(_tool_handler)
{
    m_point_size = 0.5;
}

LineMeasurementTool::~LineMeasurementTool()
{

}


void LineMeasurementTool::draw()
{
    m_measur_type = "Distance measurement";

    if(m_measurement_pt->size() == 1)
    {
        m_measurement_counter++;

        // point
        QString point_name = QString("measurement_%1point_%2").arg(m_measurement_counter).arg(m_measurement_pt->size());
        osg::Vec4 color(1.0f,0.0f,0.0f,1.0f);
        drawPoint(m_measurement_pt->back(),color,point_name);


    }
    else if(m_measurement_pt->size() >= 2)
    {

        // points
        QString point_name = QString("measurement_%1point_%2").arg(m_measurement_counter).arg(m_measurement_pt->size());

        osg::Vec4 color(1.0f,0.0f,0.0f,1.0f);
        drawPoint(m_measurement_pt->back(),color,point_name);

        // lines

        m_lines_counter++;

        QString line_name = QString("measurement_%1line_%2").arg(m_measurement_counter).arg(m_measurement_pt->size()-1);
        drawJunctionLineWithLastPoint(line_name);

    }

    m_meas_points_number[m_measurement_counter] = m_measurement_pt->size();

    m_meas_lines_number[m_measurement_counter] = m_lines_counter;

    lineLength();
}



void LineMeasurementTool::removeLastMeasurement()
{
    removeMeasurement(m_measurement_counter);
}



void LineMeasurementTool::removeMeasurement(int _meas_index)
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


double LineMeasurementTool::lineLength()
{
    if(m_measurement_pt->size() >= 2)
    {

        double x=0;
        double y=0;
        double z=0;

        double lastNorm=0;
        double norm=0;

        for(unsigned int i=1; i<m_measurement_pt->size(); ++i)
        {


            x = fabs(m_measurement_pt->at(i)[0] - m_measurement_pt->at(i-1)[0]);
            y = fabs(m_measurement_pt->at(i)[1] - m_measurement_pt->at(i-1)[1]);
            z = fabs(m_measurement_pt->at(i)[2] - m_measurement_pt->at(i-1)[2]);

            lastNorm = sqrt(x*x + y*y + z*z);
            norm += lastNorm;


        }

        m_lastNorm = lastNorm;
        m_norm = norm;

        return m_norm;

    }else
    {
        return -1.0;
    }
}

double LineMeasurementTool::closedLineLength()
{
    double open_loop_norm = lineLength();

    double x;
    double y;
    double z;

    x = fabs(m_measurement_pt->at(m_measurement_pt->size()-1)[0] - m_measurement_pt->at(0)[0]);
    y = fabs(m_measurement_pt->at(m_measurement_pt->size()-1)[1] - m_measurement_pt->at(0)[1]);
    z = fabs(m_measurement_pt->at(m_measurement_pt->size()-1)[2] - m_measurement_pt->at(0)[2]);

    m_lastNorm = sqrt(x*x + y*y + z*z);
    m_norm = m_lastNorm + open_loop_norm;

    return m_norm;

}



QString LineMeasurementTool::getTypeOfMeasur()
{
    return m_measur_type;
}


int LineMeasurementTool::getMeasurementCounter() const
{
    return m_measurement_counter;
}


void LineMeasurementTool::resetLineData()
{
    m_lastNorm = 0;
    m_norm = 0;
}

double LineMeasurementTool::getPointSize() const
{
    return m_point_size;
}

void LineMeasurementTool::setPointSize(double point_size)
{
    m_point_size = point_size;
}






