#ifndef OSG_MEASUREMENT_ROW_H
#define OSG_MEASUREMENT_ROW_H

#include <Measurement/measurement_pattern.h>

#include <osg/Geode>

class osgMeasurementRow
{
public:
    osgMeasurementRow(MeasPattern &_pattern);

    osg::ref_ptr<osg::Geode> get(int _column);
    osg::ref_ptr<osg::Group> getGroup() { return m_group; }

    bool isVisible() const { return m_visible; }
    void setVisible(const bool _visible) { m_visible = _visible; }

private:
    std::vector<osg::ref_ptr<osg::Geode>> m_geodes;
    osg::ref_ptr<osg::Group> m_group;
    bool m_visible;
};

#endif // OSG_MEASUREMENT_ROW_H
