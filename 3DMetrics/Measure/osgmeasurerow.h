#ifndef OSGMEASUREROW_H
#define OSGMEASUREROW_H

#include <Measure/measurepattern.h>

#include <osg/Geode>

class osgMeasureRow
{
public:
    osgMeasureRow(MeasurePattern &pattern);

    osg::ref_ptr<osg::Geode> get(int column);
    osg::ref_ptr<osg::Group> getGroup() { return m_group; }

private:
    std::vector<osg::ref_ptr<osg::Geode>> m_geodes;
    osg::ref_ptr<osg::Group> m_group;
};

#endif // OSGMEASUREROW_H
