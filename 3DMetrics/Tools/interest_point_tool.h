#ifndef INTERESTPOINTTOOL_H
#define INTERESTPOINTTOOL_H

#include "measurement_tool.h"

class InterestPointTool : public MeasurementTool
{
public:
    InterestPointTool(ToolHandler *_tool_handler);
    ~InterestPointTool();

    void draw();

    void endMeasurement();
    void cancelMeasurement();
    void removeLastMeasurement();
    void removeMeasurement(int _meas_index);

    QString interestPointCoordinates();
    int getMeasurementCounter() const;
    void hideShowMeasurement( int _meas_index, bool _visible);
    void closeLoop();

    QString getTextFormattedResult();

    void encodeToJSON(QJsonObject & _root_obj);
    void decodeJSON(QJsonObject & _root_obj);

    virtual void onMousePress(Qt::MouseButton _button, int _x, int _y);


private:
    QString m_coordinates;

};

#endif // INTERESTPOINTTOOL_H
