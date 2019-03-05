#include "osg_measurement_row.h"

osgMeasurementRow::osgMeasurementRow(MeasurePattern &pattern) : m_visible(true)
{
    m_group = new osg::Group();
    for(int i=0; i<pattern.getNbFields(); i++)
    {
        osg::ref_ptr<osg::Geode> p = 0;
        if(pattern.fieldType(i)!=MeasureType::String && pattern.fieldType(i)!=MeasureType::Category)
        {
            p = new osg::Geode();
            m_group->addChild(p);
        }
        m_geodes.push_back(p);
    }
}

osg::ref_ptr<osg::Geode> osgMeasurementRow::get(int column)
{
    if(column >= 0 && column < (int)m_geodes.size())
    {
        return m_geodes[column];
    }

    osg::ref_ptr<osg::Geode> p = 0;

    return p;
}
