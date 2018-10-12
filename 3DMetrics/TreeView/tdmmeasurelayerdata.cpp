#include "tdmmeasurelayerdata.h"
#include "Measure/osgmeasurerow.h"

TDMMeasureLayerData::TDMMeasureLayerData()
{
}

TDMMeasureLayerData::TDMMeasureLayerData(const QString _fileName, MeasurePattern _pattern, const osg::ref_ptr<osg::Group> _group)
{
    m_file_name = _fileName;
    m_pattern = _pattern;
    m_group = _group;
}


TDMMeasureLayerData::TDMMeasureLayerData(const TDMMeasureLayerData &_other)
{
    m_file_name = _other.m_file_name;
    m_pattern = _other.m_pattern;
    m_group = _other.m_group;
    m_rows = _other.m_rows;
}

TDMMeasureLayerData::~TDMMeasureLayerData() {}

void TDMMeasureLayerData::deleteRow(int row)
{
    osgMeasureRow *mrow = m_rows[row];
    m_group->removeChild( mrow->getGroup() );
    m_rows.erase(m_rows.begin() + row);
}

void TDMMeasureLayerData::addRow(osgMeasureRow* _row, int row)
{
    m_rows.insert(m_rows.begin() + row, _row);
    m_group->addChild(_row->getGroup());
}

