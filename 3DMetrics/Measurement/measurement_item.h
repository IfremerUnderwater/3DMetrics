#ifndef MEASUREITEM_H
#define MEASUREITEM_H

#include <QString>
#include <QJsonObject>

#include <osg/Geode>

class MeasureItem
{
public:
    MeasureItem(const QString _fieldName, osg::ref_ptr<osg::Geode> _geode);
    virtual ~MeasureItem();

    virtual QString type() = 0;

    // from JSon to object
    virtual void decode(QJsonObject & _obj) = 0;

    // encode to JSon
    virtual void encode(QJsonObject & _obj) = 0;

    // encode to ASCII
    virtual void encodeASCII(QString & _string) = 0;

    QString fieldName() const { return m_fieldName; }

    // create / update Geode
    virtual void updateGeode() = 0;

protected:
    QString m_fieldName;
    osg::ref_ptr<osg::Geode> m_geode;
};

#endif // MEASUREITEM_H
