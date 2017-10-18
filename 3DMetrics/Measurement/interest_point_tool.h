#ifndef INTERESTPOINTTOOL_H
#define INTERESTPOINTTOOL_H

#include "measurement_tool.h"

class InterestPointTool : public MeasurementTool
{
public:
    InterestPointTool();
    ~InterestPointTool();
    void draw(osg::ref_ptr<osg::Geode> &_measurement_geode);
    void removeLastMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode);
    void removeMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode, int _meas_index);
    QString interestPointCoordinates();
    QString getTypeOfMeasur();
    int getMeasurementCounter() const;
    void hideShowMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode, int _meas_index, bool _visible);
    void closeLoop(osg::ref_ptr<osg::Geode> &_measurement_geode);
    void resetInterestPointData();


private:
    QString m_coordinates;

};

#endif // INTERESTPOINTTOOL_H
