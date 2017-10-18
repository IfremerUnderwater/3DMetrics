#include "measurement_tool.h"


MeasurementTool::MeasurementTool():m_measurement_counter(0), m_lines_counter(0)
{
    m_measurement_pt = NULL;
}

MeasurementTool::~MeasurementTool()
{

}


void MeasurementTool::pushNewPoint(osg::ref_ptr<osg::Geode> & _measurement_geode, osg::Vec3d _point )
{

    // We insert the point into m_measurePoints vector.
    if(m_measurement_pt == NULL)
        m_measurement_pt = new osg::Vec3dArray;

    m_measurement_pt->push_back(_point);

    // Ask for drawing
    draw(_measurement_geode);


}

int MeasurementTool::getNumberOfPoints()
{
    return m_measurement_pt->size();
}


void MeasurementTool::hideShowMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode, int _meas_index, bool _visible)
{
    if (_visible == true)
    {
        for(int i=1; i<=m_meas_points_number[_meas_index]; ++i)
        {
            QString point_number = QString("measurement_%1point_%2").arg(_meas_index).arg(i);

            if(!_measurement_geode->containsDrawable(m_point_qmap[point_number]))
                _measurement_geode->addDrawable(m_point_qmap[point_number]);
        }

        for(int j=1; j<=m_meas_points_number[_meas_index]; ++j)
        {
            QString line_number = QString("measurement_%1line_%2").arg(_meas_index).arg(j);

            if(!_measurement_geode->containsDrawable(m_point_qmap[line_number]))
                _measurement_geode->addDrawable(m_point_qmap[line_number]);
        }
    }
    else
    {
        for(int i=1; i<=m_meas_points_number[_meas_index]; ++i)
        {
            QString point_number = QString("measurement_%1point_%2").arg(_meas_index).arg(i);

            if(_measurement_geode->containsDrawable(m_point_qmap[point_number]))
                _measurement_geode->removeDrawable(m_point_qmap[point_number]);
        }

        for(int j=1; j<=m_meas_points_number[_meas_index]; ++j)
        {
            QString line_number = QString("measurement_%1line_%2").arg(_meas_index).arg(j);

            if(_measurement_geode->containsDrawable(m_point_qmap[line_number]))
                _measurement_geode->removeDrawable(m_point_qmap[line_number]);
        }
    }
}


void MeasurementTool::closeLoop(osg::ref_ptr<osg::Geode> &_measurement_geode)
{
    if(m_measurement_pt->size() >= 3)
    {
        m_lines_counter++;

        // lines

        int current_point = (int) m_measurement_pt->size();

        QString line_number = QString("measurement_%1line_%2").arg(m_measurement_counter).arg(m_measurement_pt->size());

        osg::DrawElementsUInt* line =
                new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
        line->push_back(0);
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

        m_meas_points_number[m_measurement_counter] = m_measurement_pt->size();

        m_meas_lines_number[m_measurement_counter] = m_lines_counter;

    }
}



void MeasurementTool::resetModelData()
{
    m_point_qmap.clear();
    m_meas_points_number.clear();
    m_measurement_counter=0;
    m_measur_type.clear();
    m_measurement_pt = NULL;
    m_measurements_history_qmap.clear();
    m_lines_counter=0;
    m_meas_lines_number.clear();
}



void MeasurementTool::endMeasurement()
{
    qDebug() << "Test m_lines_counter : " << m_lines_counter;
    qDebug() << "Test m_meas_lines_number : " << m_meas_lines_number[m_measurement_counter];
    m_measurements_history_qmap[m_measurement_counter] = m_measurement_pt;
    m_measurement_pt = NULL;
    m_lines_counter=0;
}



QMap<int, osg::ref_ptr<osg::Vec3dArray> > MeasurementTool::getMeasurementsHistoryQmap()
{
    return m_measurements_history_qmap;
}


QMap<int,int> MeasurementTool::getMeasurPtsNumber()
{
    return m_meas_points_number;
}


QMap<int,int> MeasurementTool::getMeasurLinesNumber()
{
    return m_meas_lines_number;
}















