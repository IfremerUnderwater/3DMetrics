#ifndef TOOL_TYPES_H
#define TOOL_TYPES_H

#include <QMetaType>

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

struct MeasInfo {
  QString name;
  ToolState type;
  int index;
  QString category;
  QString temperature;
  QString formatted_result;
  QString comments;
} ;

Q_DECLARE_METATYPE(MeasInfo)

#endif // TOOL_TYPES_H

