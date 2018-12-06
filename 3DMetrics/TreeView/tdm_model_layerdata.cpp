#include "tdm_model_layerdata.h"

TDMModelLayerData::TDMModelLayerData() {}

TDMModelLayerData::TDMModelLayerData(const QString _fileName, const osg::ref_ptr<osg::Node> _node)
{
    m_file_name = _fileName;
    m_node = _node;
}

TDMModelLayerData::TDMModelLayerData(const TDMModelLayerData &_other)
{
    m_file_name = _other.m_file_name;
    m_node = _other.m_node;
}
TDMModelLayerData::~TDMModelLayerData() {}
