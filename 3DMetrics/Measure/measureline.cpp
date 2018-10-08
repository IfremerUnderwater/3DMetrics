#include "measureline.h"

#include <math.h>

MeasureLine::MeasureLine(const QString _fieldName) : MeasureItem(_fieldName)
{

}

void MeasureLine::computeLength()
{
    double length = 0;

    for(unsigned int i=1; i<m_array.length(); i++)
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

}

// encode to JSon
void MeasureLine::encode(QJsonObject & _obj)
{

}
