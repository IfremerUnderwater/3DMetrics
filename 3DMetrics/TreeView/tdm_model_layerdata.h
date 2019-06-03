#ifndef TDM_MODEL_LAYER_DATA_H
#define TDM_MODEL_LAYER_DATA_H

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
    TDMModelLayerData(const QString _filename, const osg::ref_ptr<osg::Node> _node);
    TDMModelLayerData(const TDMModelLayerData &_other);
    ~TDMModelLayerData();

    osg::ref_ptr<osg::Node> node() const { return m_node; }
    QString fileName() const { return m_filename; }

private:
    QString m_filename;
    osg::ref_ptr<osg::Node> m_node;
};

// needed for using with QVariant
Q_DECLARE_METATYPE(TDMModelLayerData)

#endif // TDM_MODEL_LAYER_DATA_H
