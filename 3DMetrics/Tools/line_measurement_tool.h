#ifndef LINE_MEASUREMENT_TOOL_H
#define LINE_MEASUREMENT_TOOL_H

#include "measurement_tool.h"


class LineMeasurementTool : public MeasurementTool
{
public:
    LineMeasurementTool();
    ~LineMeasurementTool();
    void draw();
    void removeLastMeasurement();
    void removeMeasurement(int _meas_index);
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
