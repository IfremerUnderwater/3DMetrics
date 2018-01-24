#ifndef INTERESTPOINTTOOL_H
#define INTERESTPOINTTOOL_H

#include "measurement_tool.h"

class InterestPointTool : public MeasurementTool
{
public:
    InterestPointTool();
    ~InterestPointTool();
    void draw();
    void removeLastMeasurement();
    void removeMeasurement(int _meas_index);
    QString interestPointCoordinates();
    QString getTypeOfMeasur();
    int getMeasurementCounter() const;
    void hideShowMeasurement( int _meas_index, bool _visible);
    void closeLoop();
    void resetInterestPointData();


private:
    QString m_coordinates;

};

#endif // INTERESTPOINTTOOL_H