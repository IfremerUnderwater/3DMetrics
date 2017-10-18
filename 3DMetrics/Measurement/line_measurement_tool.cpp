#include "line_measurement_tool.h"
#include <math.h>

LineMeasurementTool::LineMeasurementTool():MeasurementTool()
{
    m_point_size = 0.01;
}

LineMeasurementTool::~LineMeasurementTool()
{

}


void LineMeasurementTool::draw(osg::ref_ptr<osg::Geode> &_measurement_geode)
{
    m_measur_type = "Distance measurement";

    if(m_measurement_pt->size() == 1)
    {
        m_measurement_counter++;

        // point
        QString point_number = QString("measurement_%1point_%2").arg(m_measurement_counter).arg(m_measurement_pt->size());

        osg::ref_ptr<osg::ShapeDrawable> shape_point_drawable = new osg::ShapeDrawable(new osg::Sphere(m_measurement_pt->back(),m_point_size));
        _measurement_geode->addDrawable(shape_point_drawable);
        m_point_qmap[point_number] = shape_point_drawable;    

    }
    else if(m_measurement_pt->size() >= 2)
    {

        // points

        QString point_number = QString("measurement_%1point_%2").arg(m_measurement_counter).arg(m_measurement_pt->size());

        osg::ref_ptr<osg::ShapeDrawable> shape_point_drawable = new osg::ShapeDrawable(new osg::Sphere(m_measurement_pt->back(),m_point_size));
        _measurement_geode->addDrawable(shape_point_drawable);

        m_point_qmap[point_number] = shape_point_drawable;


        // lines

        m_lines_counter++;

        int current_point = (int) m_measurement_pt->size();

        QString line_number = QString("measurement_%1line_%2").arg(m_measurement_counter).arg(m_measurement_pt->size()-1);

        osg::DrawElementsUInt* line =
                new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
        line->push_back(current_point-2);
        line->push_back(current_point-1);

        osg::Geometry* geoPoints = new osg::Geometry;
        geoPoints->setVertexArray(m_measurement_pt);
        geoPoints->addPrimitiveSet(line);


        osg::Vec4dArray* tabCouleur = new osg::Vec4dArray;
        tabCouleur->push_back(osg::Vec4d(1.0f, 0.0f, 0.0f, 1.0f)); //red
        geoPoints->setColorArray(tabCouleur);

        geoPoints->setColorBinding(osg::Geometry::BIND_OVERALL);

        _measurement_geode->addDrawable(geoPoints);

        m_point_qmap[line_number] = geoPoints;

    }

    m_meas_points_number[m_measurement_counter] = m_measurement_pt->size();

    m_meas_lines_number[m_measurement_counter] = m_lines_counter;

    lineLength();
}



void LineMeasurementTool::removeLastMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode)
{
    removeMeasurement(_measurement_geode, m_measurement_counter);
}



void LineMeasurementTool::removeMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode, int _meas_index)
{

    for(int i=1; i<=m_meas_points_number[_meas_index]; ++i)
    {
        QString point_number = QString("measurement_%1point_%2").arg(_meas_index).arg(i);
        _measurement_geode->removeDrawable(m_point_qmap[point_number]);
        m_point_qmap.remove(point_number);
    }

    for(int j=1; j<=m_meas_points_number[_meas_index]; ++j)
    {
        QString line_number = QString("measurement_%1line_%2").arg(_meas_index).arg(j);
        _measurement_geode->removeDrawable(m_point_qmap[line_number]);
        m_point_qmap.remove(line_number);
    }


    m_measurements_history_qmap.remove(_meas_index);

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






