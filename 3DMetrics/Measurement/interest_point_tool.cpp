#include "interest_point_tool.h"
#include <math.h>

InterestPointTool::InterestPointTool()
{

}


InterestPointTool::~InterestPointTool()
{

}


void InterestPointTool::draw(osg::ref_ptr<osg::Geode> &_measurement_geode)
{

    if(m_measurement_pt->size() >= 1)
    {
        m_measurement_counter++;

        m_measur_type = "Interest point measurement";

        // point
        QString point_name = QString("measurement_%1").arg(m_measurement_counter);

        osg::Vec4 color(0.0f,0.0f,1.0f,1.0f);
        drawPoint(_measurement_geode,m_measurement_pt->back(),color,point_name);

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


void InterestPointTool::removeLastMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode)
{
    removeMeasurement(_measurement_geode, m_measurement_counter);
}


void InterestPointTool::removeMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode, int _meas_index)
{
    QString point_number = QString("measurement_%1").arg(_meas_index);

    _measurement_geode->removeDrawable(m_geo_drawable_map[point_number]);
    m_geo_drawable_map.remove(point_number);

    m_measurements_history_qmap.remove(_meas_index);

    m_measurement_counter--;

}


QString InterestPointTool::getTypeOfMeasur()
{
    return m_measur_type;
}


int InterestPointTool::getMeasurementCounter() const
{
    return m_measurement_counter;
}


void InterestPointTool::hideShowMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode, int _meas_index, bool _visible)
{

    if (_visible)
    {
        QString point_number = QString("measurement_%1").arg(_meas_index);

        if(!_measurement_geode->containsDrawable(m_geo_drawable_map[point_number]))
        {
            _measurement_geode->addDrawable(m_geo_drawable_map[point_number]);
            qDebug() << "Add measur";
        }
    }
    else
    {
        QString point_number = QString("measurement_%1").arg(_meas_index);

        if(_measurement_geode->containsDrawable(m_geo_drawable_map[point_number]))
        {
            _measurement_geode->removeDrawable(m_geo_drawable_map[point_number]);
            qDebug() << "Remove measur";
        }
    }
}



void InterestPointTool::closeLoop(osg::ref_ptr<osg::Geode> &_measurement_geode)
{
    // this method is not used in this class
}



void InterestPointTool::resetInterestPointData()
{
    m_coordinates.clear();
}

