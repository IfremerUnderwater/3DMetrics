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

    double getTransparency() const { return m_transparency_value; }
    void setTransparencyValue(const double _transparency_value) { m_transparency_value = _transparency_value; }

    double getOffsetX() const { return m_offsetX; }
    void setOffsetX(double offsetX) { m_offsetX = offsetX; }

    double getOffsetY() const { return m_offsetY; }
    void setOffsetY(double offsetY){ m_offsetY = offsetY; }

    double getOffsetZ() const { return m_offsetZ; }
    void setOffsetZ(double offsetZ){ m_offsetZ = offsetZ; }

private:
    QString m_filename;
    osg::ref_ptr<osg::Node> m_node;

    double m_transparency_value;

    double m_offsetX;
    double m_offsetY;
    double m_offsetZ;
};

// needed for using with QVariant
Q_DECLARE_METATYPE(TDMModelLayerData)

#endif // TDM_MODEL_LAYER_DATA_H
