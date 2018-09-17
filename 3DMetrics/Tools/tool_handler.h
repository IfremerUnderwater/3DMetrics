#ifndef TOOLHANDLER_H
#define TOOLHANDLER_H

#include "measurement_tool.h"
#include "line_measurement_tool.h"
#include "surface_measurement_tool.h"
#include "interest_point_tool.h"
#include "tool_types.h"

#include <osg/ref_ptr>
#include <osg/Geode>

class OSGWidget;

class ToolHandler:public QObject
{
    Q_OBJECT
public:
    ToolHandler(OSGWidget *_osg_widget = NULL);

    void setCurrentToolState(ToolState _tool_state);

    void cancelMeasurement();
    void removeLastMeasurement();
    void removeMeasurementOfType(ToolState _meas_type, int _meas_index);

    // hide/show measurement method
    void hideShowMeasurementOfType(ToolState _meas_type, int _meas_index, bool _visible);

    //void setOsgWidget(OSGWidget *_osg_widget);

    // getIntersectionPoint from OSGWidget
    void getIntersectionPoint(int _x, int _y, osg::Vec3d &_inter_point, bool &_inter_exists);

    // update Geode drawing after modification
    void forceGeodeUpdate();

    void emitMeasurementEnded();

    void newMeasEndedWithInfo(MeasInfo _meas_info);

    ToolState getCurrentState() const;

    QString getTextFormattedResult();

    QPair<ToolState, int> getMeasTypeAndIndex();

    void encodeToJSON(QJsonObject & _root_obj);
    void decodeJSON(QJsonObject & _root_obj);

    void setCurrentMeasInfo(QString _name, QString _comment, QString _temp, QString _category);

    void getGeoOrigin(QPointF &_ref_lat_lon, double &_ref_depth);

    void resetMeasData();

    osg::ref_ptr<osg::Geode> getGeode() const { return m_geode; }

public slots:
    void slot_onMousePress(Qt::MouseButton _button ,int _x, int _y);

signals:
    void measurementEnded();
    void sig_newMeasEndedWithInfo(MeasInfo _meas_info);

private:
    MeasurementTool *m_current_tool;

    LineMeasurementTool m_line_meas_tool;
    SurfaceMeasurementTool m_surf_meas_tool;
    InterestPointTool m_interest_point_tool;

    ToolState m_current_toolstate;

    OSGWidget *m_osg_widget;

    osg::ref_ptr<osg::Geode> m_geode;
};

#endif // TOOLHANDLER_H
