#include "measurement_item.h"

MeasItem::MeasItem(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode)
    : m_field_name(_fieldName)
    , m_geode(_geode)
{
}

MeasItem::~MeasItem()
{
}
