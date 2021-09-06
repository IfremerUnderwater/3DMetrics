#ifndef TDM_MODEL_LAYER_DATA_H
#define TDM_MODEL_LAYER_DATA_H

#include <osg/ref_ptr>
#include <osg/Node>
#include <QString>
#include <QMetaType>
#include "OSGWidget/loading_mode.h"

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

    osg::Vec3d getOriginalTranslation() const { return m_trans; }

    void setLODThreshold(float _th1, float _th2)
    {
        m_threshold1 = _th1;
        m_threshold2 = _th2;
    }

    float getThreshold1() const { return m_threshold1; }
    float getThreshold2() const { return m_threshold2; }

    LoadingMode getLoadingMode() const { return m_loadingMode; }
    void setLoadingMode(const LoadingMode _loadingMode) { m_loadingMode = _loadingMode; }

    QString getRelativeItemsDir() const { return m_relativeItemsDir; }
    void setRelativeItemsDir(const QString &_relativeItemsDir) { m_relativeItemsDir = _relativeItemsDir; }

private:
    QString m_filename;
    osg::ref_ptr<osg::Node> m_node;
    osg::Vec3d m_trans;

    double m_transparency_value;

    double m_offsetX;
    double m_offsetY;
    double m_offsetZ;

    QString m_relativeItemsDir;

    float m_threshold1;
    float m_threshold2;

    LoadingMode m_loadingMode;
};

// needed for using with QVariant
Q_DECLARE_METATYPE(TDMModelLayerData)

#endif // TDM_MODEL_LAYER_DATA_H
