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

void LineMeasurementTool::endMeasurement()
{
    // Compute lineLength and affect it in history map
    m_measurements_length[m_last_meas_idx] = lineLength();

    // Call parent method
    MeasurementTool::endMeasurement();
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

    m_last_meas_idx--;

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






