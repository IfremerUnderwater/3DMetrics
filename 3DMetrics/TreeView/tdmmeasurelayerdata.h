#ifndef TDMMEASURELAYERDATA_H
#define TDMMEASURELAYERDATA_H

#include <QString>
#include <QMetaType>

//#include "tool_handler.h"

#include "Measure/measurepattern.h"

#include <memory>

// private data used in Measure Layer
// usable in QVariant
// stored in hidden colums in the treeitem
//
class TDMMeasureLayerData
{
public:
    TDMMeasureLayerData();
    //TDMMeasureLayerData(const QString _fileName, std::shared_ptr<ToolHandler> _tool);
    TDMMeasureLayerData(const QString _fileName, MeasurePattern _pattern);
    TDMMeasureLayerData(const TDMMeasureLayerData &_other);
    ~TDMMeasureLayerData();

    QString fileName() const { return m_file_name; }
    void setFileName(QString _name) { m_file_name = _name; }
    //std::shared_ptr<ToolHandler> tool() const { return m_tool; }
    MeasurePattern & pattern() { return m_pattern; }
    //void setPattern(MeasurePattern _pattern) { m_pattern = _pattern; }

private:
    QString m_file_name; // may be empty when just created
    MeasurePattern m_pattern;
//    std::shared_ptr<ToolHandler> m_tool;
};

// needed for using with QVariant
Q_DECLARE_METATYPE(TDMMeasureLayerData)

#endif // TDMMEASURELAYERDATA_H
