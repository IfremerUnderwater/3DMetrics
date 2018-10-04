#include "tdmmeasurelayerdata.h"

TDMMeasureLayerData::TDMMeasureLayerData()
{
}

TDMMeasureLayerData::TDMMeasureLayerData(const QString _fileName, MeasurePattern _pattern)
{
    m_file_name = _fileName;
    m_pattern = _pattern;
}

//TDMMeasureLayerData::TDMMeasureLayerData(const QString _fileName, std::shared_ptr<ToolHandler> _tool)
//{
//    m_file_name = _fileName;
//    m_tool = _tool;
//}

TDMMeasureLayerData::TDMMeasureLayerData(const TDMMeasureLayerData &_other)
{
    m_file_name = _other.m_file_name;
    m_pattern = _other.m_pattern;
}

TDMMeasureLayerData::~TDMMeasureLayerData() {}
