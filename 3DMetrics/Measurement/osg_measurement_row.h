#ifndef OSGMEASUREROW_H
#define OSGMEASUREROW_H

#include <Measurement/measurement_pattern.h>

#include <osg/Geode>

class osgMeasurementRow
{
public:
    osgMeasurementRow(MeasurePattern &pattern);

    osg::ref_ptr<osg::Geode> get(int column);
    osg::ref_ptr<osg::Group> getGroup() { return m_group; }

    bool isVisible() const { return m_visible; }
    void setVisible(const bool _visible) { m_visible = _visible; }

private:
    std::vector<osg::ref_ptr<osg::Geode>> m_geodes;
    osg::ref_ptr<osg::Group> m_group;

    bool m_visible;
};

#endif // OSGMEASUREROW_H
