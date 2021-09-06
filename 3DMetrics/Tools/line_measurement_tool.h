#ifndef LINE_MEASUREMENT_TOOL_H
#define LINE_MEASUREMENT_TOOL_H

#include "measurement_tool.h"


class LineMeasurementTool : public MeasurementTool
{
public:
    LineMeasurementTool(ToolHandler *_tool_handler);
    ~LineMeasurementTool();
    void draw();

    void cancelMeasurement();
    void endMeasurement(bool _meas_info_is_set=false);
    void removeLastMeasurement();
    void removeMeasurement(int _meas_index);

    double lineLength();
    int getMeasurementCounter() const;
    QString getTextFormattedResult();

    void encodeToJSON(QJsonObject & _root_obj);
    void decodeJSON(QJsonObject & _root_obj);

    virtual void onMousePress(Qt::MouseButton _button, int _x, int _y);

private:
    double m_norm;

    // Map to keep history of measurements results
    QMap<int, double > m_measurements_length;

};

#endif // LINE_MEASUREMENT_TOOL_H