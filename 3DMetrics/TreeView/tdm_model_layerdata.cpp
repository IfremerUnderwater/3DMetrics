#include "tdm_model_layerdata.h"

TDMModelLayerData::TDMModelLayerData():m_transparency_value(0.0), m_offsetX(0), m_offsetY(0), m_offsetZ(0) {}

TDMModelLayerData::TDMModelLayerData(const QString _filename, const osg::ref_ptr<osg::Node> _node):
    m_transparency_value(0.0), m_offsetX(0), m_offsetY(0), m_offsetZ(0)
{
    m_filename = _filename;
    m_node = _node;
}

TDMModelLayerData::TDMModelLayerData(const TDMModelLayerData &_other)
{
    m_filename = _other.m_filename;
    m_node = _other.m_node;
    m_transparency_value = _other.m_transparency_value;
    m_offsetX = _other.m_offsetX;
    m_offsetY = _other.m_offsetY;
    m_offsetZ = _other.m_offsetZ;
}

TDMModelLayerData::~TDMModelLayerData() {}



