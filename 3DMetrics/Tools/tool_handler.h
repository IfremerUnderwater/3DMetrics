#ifndef TOOLHANDLER_H
#define TOOLHANDLER_H

#include "measurement_tool.h"
#include "line_measurement_tool.h"
#include "surface_measurement_tool.h"
#include "interest_point_tool.h"

// Various states according to the action clicked
enum ToolState
{
    IDLE_STATE,
    LINE_MEASUREMENT_STATE,
    SURFACE_MEASUREMENT_STATE,
    INTEREST_POINT_STATE,
    CUT_AREA_TOOL_STATE,
    ZOOM_IN_TOOL_STATE,
    ZOOM_OUT_TOOL_STATE,
    FULL_SCREEN_TOOL_STATE,
    CROP_TOOL_STATE
};

class OSGWidget;

class ToolHandler:public QObject
{
    Q_OBJECT
public:
    ToolHandler();

    void setCurrentToolState(ToolState _tool_state);

    void cancelMeasurement();
    void removeMeasurementOfType(ToolState _meas_type, int _meas_index);

    // hide/show measurement method
    void hideShowMeasurementOfType(ToolState _meas_type, int _meas_index, bool _visible);

    void setOsgWidget(OSGWidget *_osg_widget);

    // getIntersectionPoint from OSGWidget
    void getIntersectionPoint(int _x, int _y, osg::Vec3d &_inter_point, bool &_inter_exists);

public slots:
    void slot_onMousePress(Qt::MouseButton _button ,int _x, int _y);

private:
    MeasurementTool *m_current_tool;

    LineMeasurementTool m_line_meas_tool;
    SurfaceMeasurementTool m_surf_meas_tool;
    InterestPointTool m_interest_point_tool;

    ToolState m_current_toolstate;

    OSGWidget *m_osg_widget;
};

#endif // TOOLHANDLER_H
