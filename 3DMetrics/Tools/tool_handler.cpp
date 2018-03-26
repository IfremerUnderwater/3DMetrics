#include "tool_handler.h"
#include "OSGWidget/OSGWidget.h"

ToolHandler::ToolHandler():m_line_meas_tool(this),
    m_surf_meas_tool(this),
    m_interest_point_tool(this)
{
    m_current_toolstate = IDLE_STATE;
    m_current_tool = NULL;

}

void ToolHandler::setCurrentToolState(ToolState _tool_state)
{
    m_current_toolstate = _tool_state;

    switch (_tool_state) {
    case IDLE_STATE:
        m_current_tool = NULL;
        break;
    case LINE_MEASUREMENT_STATE:
        m_current_tool = &m_line_meas_tool;
        break;
    case SURFACE_MEASUREMENT_STATE:
        m_current_tool = &m_surf_meas_tool;
        break;
    case INTEREST_POINT_STATE:
        m_current_tool = &m_interest_point_tool;
        break;
    default:
        break;
    }
}

void ToolHandler::cancelMeasurement()
{
    if(m_current_tool)
        m_current_tool->cancelMeasurement();
}

void ToolHandler::removeLastMeasurement()
{
    if(m_current_tool)
        m_current_tool->removeLastMeasurement();
}

void ToolHandler::removeMeasurementOfType(ToolState _meas_type, int _meas_index)
{
    switch (_meas_type) {
    case LINE_MEASUREMENT_STATE:
        m_line_meas_tool.removeMeasurement(_meas_index);
        break;
    case SURFACE_MEASUREMENT_STATE:
        m_surf_meas_tool.removeMeasurement(_meas_index);
        break;
    case INTEREST_POINT_STATE:
        m_interest_point_tool.removeMeasurement(_meas_index);
        break;
    default:
        break;
    }
}

void ToolHandler::hideShowMeasurementOfType(ToolState _meas_type, int _meas_index, bool _visible)
{
    switch (_meas_type) {
    case LINE_MEASUREMENT_STATE:
        m_line_meas_tool.hideShowMeasurement(_meas_index,_visible);
        break;
    case SURFACE_MEASUREMENT_STATE:
        m_surf_meas_tool.hideShowMeasurement(_meas_index,_visible);
        break;
    case INTEREST_POINT_STATE:
        m_interest_point_tool.hideShowMeasurement(_meas_index,_visible);
        break;
    default:
        break;
    }
}

void ToolHandler::slot_onMousePress(Qt::MouseButton _button, int _x, int _y)
{
    if(m_current_tool)
        m_current_tool->onMousePress(_button,_x,_y);
}

ToolState ToolHandler::getCurrentState() const
{
    return m_current_toolstate;
}

QString ToolHandler::getTextFormattedResult()
{
    if(m_current_toolstate!=IDLE_STATE)
        return m_current_tool->getTextFormattedResult();
    else
        return QString("");
}

QPair<ToolState, int> ToolHandler::getMeasTypeAndIndex()
{
    return m_current_tool->getMeasTypeAndIndex();
}

void ToolHandler::encodeToJSON(QJsonObject & _root_obj)
{
    m_line_meas_tool.encodeToJSON(_root_obj);
    m_surf_meas_tool.encodeToJSON(_root_obj);
    m_interest_point_tool.encodeToJSON(_root_obj);
}

void ToolHandler::decodeJSON(QJsonObject & _root_obj)
{
    m_line_meas_tool.decodeJSON(_root_obj);
    m_surf_meas_tool.decodeJSON(_root_obj);
    m_interest_point_tool.decodeJSON(_root_obj);
}

void ToolHandler::setCurrentMeasInfo(QString _name, QString _comment, QString _temp, QString _category)
{
    m_current_tool->setCurrentMeasInfo(_name, _comment, _temp, _category);
}

void ToolHandler::getGeoOrigin(QPointF &_ref_lat_lon, double &_ref_depth)
{
    m_osg_widget->getGeoOrigin(_ref_lat_lon,_ref_depth);
}

void ToolHandler::setOsgWidget(OSGWidget *_osg_widget)
{
    m_osg_widget = _osg_widget;

    m_line_meas_tool.setMeasurementGeode(m_osg_widget->getMeasurementGeode());
    m_surf_meas_tool.setMeasurementGeode(m_osg_widget->getMeasurementGeode());
    m_interest_point_tool.setMeasurementGeode(m_osg_widget->getMeasurementGeode());
}

void ToolHandler::getIntersectionPoint(int _x, int _y, osg::Vec3d &_inter_point, bool &_inter_exists)
{
    m_osg_widget->getIntersectionPoint(_x, _y, _inter_point, _inter_exists);
}

void ToolHandler::forceGeodeUpdate()
{
    m_osg_widget->forceGeodeUpdate();
}

void ToolHandler::emitMeasurementEnded()
{
    emit measurementEnded();
}

void ToolHandler::newMeasEndedWithInfo(MeasInfo _meas_info)
{
    emit sig_newMeasEndedWithInfo(_meas_info);
}

