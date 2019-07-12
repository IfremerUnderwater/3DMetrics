#ifndef MEAS_ITEM_H
#define MEAS_ITEM_H

#include <QString>
#include <QJsonObject>

#include <osg/Geode>

class MeasItem
{
public:
    MeasItem(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode);
    virtual ~MeasItem();

    virtual QString type() = 0;

    // from JSon to object
    virtual void decode(QJsonObject & _obj) = 0;

    // encode to JSon
    virtual void encode(QJsonObject & _obj) = 0;

    // encode to ASCII
    virtual void encodeASCII(QString & _string) = 0;

    // encode to Shapefile
    virtual void encodeShapefile(QString & _string) = 0;

    QString fieldName() const { return m_field_name; }

    // create / update Geode
    virtual void updateGeode() = 0;

protected:
    QString m_field_name;
    osg::ref_ptr<osg::Geode> m_geode;
};

#endif // MEAS_ITEM_H
