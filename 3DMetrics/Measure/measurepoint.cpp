#include "measurepoint.h"

MeasurePoint::MeasurePoint(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode)
    : MeasureItem(_fieldName, _geode),
      m_p()
{

}

// from JSon to object
void MeasurePoint::decode(QJsonObject & _obj)
{
  QJsonObject p = _obj.value(fieldName()).toObject();
  m_p.decode(p);
}

// encode to JSon
void MeasurePoint::encode(QJsonObject & _obj)
{
    QJsonObject p;
    m_p.encode(p);
    _obj.insert(fieldName(), QJsonValue(p));
}
