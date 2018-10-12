#include "measureline.h"

#include <math.h>
#include <QJsonArray>

MeasureLine::MeasureLine(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode)
    : MeasureItem(_fieldName, _geode)
    , m_length(0)
{

}

void MeasureLine::computeLength()
{
    double length = 0;

    for(int i=1; i<m_array.length(); i++)
    {
        double dx = m_array[i].x - m_array[i-1].x;
        double dy = m_array[i].y - m_array[i-1].y;
        double dz = m_array[i].z - m_array[i-1].z;

        length += sqrt(dx*dx + dy*dy + dz*dz);
    }

    m_length = length;
}


// from JSon to object
void MeasureLine::decode(QJsonObject & _obj)
{
    m_array.clear();

    QJsonObject p = _obj.value(fieldName()).toObject();
    m_length = p.value("length").toDouble();

    QJsonArray array = p.value("pts").toArray();
    for(int i=0; i<array.count(); i++)
    {
        QJsonObject op = array.at(i).toObject();
        Point3D p;
        p.decode(op);
        m_array.append(p);
    }
}

// encode to JSon
void MeasureLine::encode(QJsonObject & _obj)
{
    QJsonObject obj;

    // length
    obj.insert("length",QJsonValue(m_length));

    // pts
    QJsonArray array;
    for(int i=0; i<m_array.length(); i++)
    {
        Point3D p = m_array[i];
        QJsonObject val;
        p.encode(val);
        array.push_back(QJsonValue(val));
    }
    obj.insert("pts", array);

    // full object
    _obj.insert(fieldName(), obj);
}

void MeasureLine::updateGeode()
{

}
