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

void ToolHandler::removeLastMeasurementOfType(ToolState _meas_type)
{

}

void ToolHandler::removeMeasurementOfType(ToolState _meas_type, int _meas_index)
{

}

void ToolHandler::hideShowMeasurementOfType(ToolState _meas_type, int _meas_index, bool _visible)
{

}

void ToolHandler::onMousePress(Qt::MouseButton _button, int _x, int _y)
{

}

