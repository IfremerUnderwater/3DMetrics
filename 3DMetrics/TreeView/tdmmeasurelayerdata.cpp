#include "tdmmeasurelayerdata.h"

TDMMeasureLayerData::TDMMeasureLayerData()
{
}

TDMMeasureLayerData::TDMMeasureLayerData(const QString _fileName, std::shared_ptr<ToolHandler> _tool)
{
    m_file_name = _fileName;
    m_tool = _tool;
}

TDMMeasureLayerData::TDMMeasureLayerData(const TDMMeasureLayerData &other)
{
    m_file_name = other.m_file_name;
    m_tool = other.m_tool;
}

TDMMeasureLayerData::~TDMMeasureLayerData() {}
