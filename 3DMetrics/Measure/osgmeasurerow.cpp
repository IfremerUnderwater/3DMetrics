#include "osgmeasurerow.h"

osgMeasureRow::osgMeasureRow(MeasurePattern &pattern) : m_visible(true)
{
    m_group = new osg::Group();
    for(int i=0; i<pattern.getNbFields(); i++)
    {
        osg::ref_ptr<osg::Geode> p = 0;
        if(pattern.fieldType(i) != MeasureType::String)
        {
            p = new osg::Geode();
            m_group->addChild(p);
        }
        m_geodes.push_back(p);
    }
}

osg::ref_ptr<osg::Geode> osgMeasureRow::get(int column)
{
    if(column >= 0 && column < m_geodes.size())
    {
        return m_geodes[column];
    }

    osg::ref_ptr<osg::Geode> p = 0;

    return p;
}
