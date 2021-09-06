#include "tdm_measurement_layerdata.h"
#include "Measurement/osg_measurement_row.h"

TDMMeasurementLayerData::TDMMeasurementLayerData()
{
}

TDMMeasurementLayerData::TDMMeasurementLayerData(const QString _filename, MeasPattern _pattern, const osg::ref_ptr<osg::Group> _group)
{
    m_filename = _filename;
    m_pattern = _pattern;
    m_group = _group;
}


TDMMeasurementLayerData::TDMMeasurementLayerData(const TDMMeasurementLayerData &_other)
{
    m_filename = _other.m_filename;
    m_pattern = _other.m_pattern;
    m_group = _other.m_group;
    m_rows = _other.m_rows;
}

TDMMeasurementLayerData::~TDMMeasurementLayerData() {}

void TDMMeasurementLayerData::deleteRow(int row)
{
    osgMeasurementRow *mrow = m_rows[row];
    m_group->removeChild( mrow->getGroup() );
    m_rows.erase(m_rows.begin() + row);
}

void TDMMeasurementLayerData::addRow(osgMeasurementRow* _row, int row)
{
    m_rows.insert(m_rows.begin() + row, _row);
    m_group->addChild(_row->getGroup());
}

