#ifndef LINE_MEASUREMENT_TOOL_H
#define LINE_MEASUREMENT_TOOL_H

#include "measurement_tool.h"


class LineMeasurementTool : public MeasurementTool
{
public:
    LineMeasurementTool();
    ~LineMeasurementTool();
    void draw(osg::ref_ptr<osg::Geode> &_measurement_geode);
    void removeLastMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode);
    void removeMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode, int _meas_index);
    double lineLength();
    double closedLineLength();
    QString getTypeOfMeasur();
    int getMeasurementCounter() const;
    void resetLineData();

    double getPointSize() const;
    void setPointSize(double point_size);

private:
    double m_lastNorm;
    double m_norm;
    double m_point_size;


};

#endif // LINE_MEASUREMENT_TOOL_H
