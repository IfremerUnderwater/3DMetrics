#include "tdm_model_layerdata.h"

TDMModelLayerData::TDMModelLayerData() {}

TDMModelLayerData::TDMModelLayerData(const QString _filename, const osg::ref_ptr<osg::Node> _node)
{
    m_filename = _filename;
    m_node = _node;
}

TDMModelLayerData::TDMModelLayerData(const TDMModelLayerData &_other)
{
    m_filename = _other.m_filename;
    m_node = _other.m_node;
}
TDMModelLayerData::~TDMModelLayerData() {}
