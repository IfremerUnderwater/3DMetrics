#ifndef TDMMODELLAYERDATA_H
#define TDMMODELLAYERDATA_H

#include <osg/ref_ptr>
#include <osg/Node>
#include <QString>
#include <QMetaType>

// private data used in Model Layer
// usable in QVariant
// stored in hidden colums in the treeitem
//
class TDMModelLayerData
{
public:
    TDMModelLayerData();
    TDMModelLayerData(const QString _fileName, const osg::ref_ptr<osg::Node> _node);
    TDMModelLayerData(const TDMModelLayerData &_other);
    ~TDMModelLayerData();

    osg::ref_ptr<osg::Node> node() const { return m_node; }
    QString fileName() const { return m_file_name; }

private:
    QString m_file_name;
    osg::ref_ptr<osg::Node> m_node;
};

// needed for using with QVariant
Q_DECLARE_METATYPE(TDMModelLayerData)

#endif // TDMMODELLAYERDATA_H
