#include "osg_measurement_row.h"
// for OSGWidget::MEASURE_NAME
#include "OSGWidget/osg_widget.h"

osgMeasurementRow::osgMeasurementRow(MeasPattern &_pattern) : m_visible(true)
{
    m_group = new osg::Group();
    m_group->setName(OSGWidget::MEASURE_NAME);
    for(int i=0; i<_pattern.getNbFields(); i++)
    {
        osg::ref_ptr<osg::Geode> geode = 0;
        if(_pattern.fieldType(i)!=MeasType::String && _pattern.fieldType(i)!=MeasType::Category)
        {
            geode = new osg::Geode();
            m_group->addChild(geode);
        }
        m_geodes.push_back(geode);
    }
}

osg::ref_ptr<osg::Geode> osgMeasurementRow::get(int _column)
{
    if(_column >= 0 && _column < (int)m_geodes.size())
    {
        return m_geodes[_column];
    }

    osg::ref_ptr<osg::Geode> geode = 0;

    return geode;
}
