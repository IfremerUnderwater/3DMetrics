#include "tool_handler.h"

ToolHandler::ToolHandler()
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

void ToolHandler::cancelLastMeasurement()
{
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

void ToolHandler::onMousePress(Qt::MouseButton _button, int _x, int _y)
{
    m_current_tool->onMousePress(_button,_x,_y);
}

