#include "measurement_item.h"

MeasureItem::MeasureItem(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode)
    : m_fieldName(_fieldName)
    , m_geode(_geode)
{
}

MeasureItem::~MeasureItem()
{
}
