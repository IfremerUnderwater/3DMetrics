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

    // drawing
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

            if(!_measurement_geode->containsDrawable(m_geo_drawable_map[point_number]))
                _measurement_geode->addDrawable(m_geo_drawable_map[point_number]);
        }

        for(int j=1; j<=m_meas_points_number[_meas_index]; ++j)
        {
            QString line_number = QString("measurement_%1line_%2").arg(_meas_index).arg(j);

            if(!_measurement_geode->containsDrawable(m_geo_drawable_map[line_number]))
                _measurement_geode->addDrawable(m_geo_drawable_map[line_number]);
        }
    }
    else
    {
        for(int i=1; i<=m_meas_points_number[_meas_index]; ++i)
        {
            QString point_number = QString("measurement_%1point_%2").arg(_meas_index).arg(i);

            if(_measurement_geode->containsDrawable(m_geo_drawable_map[point_number]))
                _measurement_geode->removeDrawable(m_geo_drawable_map[point_number]);
        }

        for(int j=1; j<=m_meas_points_number[_meas_index]; ++j)
        {
            QString line_number = QString("measurement_%1line_%2").arg(_meas_index).arg(j);

            if(_measurement_geode->containsDrawable(m_geo_drawable_map[line_number]))
                _measurement_geode->removeDrawable(m_geo_drawable_map[line_number]);
        }
    }
}


void MeasurementTool::closeLoop(osg::ref_ptr<osg::Geode> &_measurement_geode)
{
    if(m_measurement_pt->size() >= 3)
    {
        m_lines_counter++;

        // lines (to be modified with line function)

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
        tabCouleur->push_back(osg::Vec4d(0.0f, 1.0f, 0.0f, 1.0f)); //red
        geoPoints->setColorArray(tabCouleur);

        geoPoints->setColorBinding(osg::Geometry::BIND_OVERALL);

        _measurement_geode->addDrawable(geoPoints);

        m_geo_drawable_map[line_number] = geoPoints;

        m_meas_points_number[m_measurement_counter] = m_measurement_pt->size();

        m_meas_lines_number[m_measurement_counter] = m_lines_counter;

    }
}



void MeasurementTool::resetModelData()
{
    m_geo_drawable_map.clear();
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

void MeasurementTool::drawPoint(osg::ref_ptr<osg::Geode> &_measurement_geode, osg::Vec3d &_point, osg::Vec4 &_color, QString _point_name)
{
    // point
    osg::Geometry* shape_point_drawable = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array;
    vertices->push_back(_point);

    // pass the created vertex array to the points geometry object.
    shape_point_drawable->setVertexArray(vertices);

    osg::Vec4Array* colors = new osg::Vec4Array;
    // add a white color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    colors->push_back(_color);

    // pass the color array to points geometry, note the binding to tell the geometry
    // that only use one color for the whole object.
    shape_point_drawable->setColorArray(colors, osg::Array::BIND_OVERALL);

    // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
    // parameter passed to the DrawArrays constructor is the Primitive::Mode which
    // in this case is POINTS (which has the same value GL_POINTS), the second
    // parameter is the index position into the vertex array of the first point
    // to draw, and the third parameter is the number of points to draw.
    shape_point_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));

    _measurement_geode->addDrawable(shape_point_drawable);
    m_geo_drawable_map[_point_name] = shape_point_drawable;
}

void MeasurementTool::drawJunctionLineWithLastPoint(osg::ref_ptr<osg::Geode> &_measurement_geode, QString &line_name)
{
    int current_point = (int) m_measurement_pt->size();

    osg::DrawElementsUInt* line =
            new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
    line->push_back(current_point-2);
    line->push_back(current_point-1);

    osg::Geometry* geoPoints = new osg::Geometry;
    geoPoints->setVertexArray(m_measurement_pt);
    geoPoints->addPrimitiveSet(line);


    osg::Vec4dArray* tabCouleur = new osg::Vec4dArray;
    tabCouleur->push_back(osg::Vec4d(0.0f, 1.0f, 0.0f, 0.7f)); //green
    geoPoints->setColorArray(tabCouleur,osg::Array::BIND_OVERALL);

    // set the normal in the same way color.
    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
    geoPoints->setNormalArray(normals, osg::Array::BIND_OVERALL);


    _measurement_geode->addDrawable(geoPoints);

    m_geo_drawable_map[line_name] = geoPoints;
}















