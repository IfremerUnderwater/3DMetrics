#ifndef FILE_OPEN_THREAD_H
#define FILE_OPEN_THREAD_H

#include <QThread>

#include <osg/Node>
#include "TreeView/tdm_layer_item.h"

#include "OSGWidget/loading_mode.h"

class OSGWidget;

class FileOpenThread : public QThread
{
    Q_OBJECT
public:
    explicit FileOpenThread();
    void run();
    //bool Stop;
    void setOSGWidget(OSGWidget *_osg_widget) {m_osg_widget = _osg_widget;}
    void setFileName(QString _filename) {m_filename = _filename;}
    void setTDMLayerItem(TdmLayerItem *_parent) {m_parent = _parent;}
    void setSelectItem(bool _select_item) {m_select_item = _select_item;}
    osg::ref_ptr<osg::Node> getNode() {return m_node;}

    void setName(QString _name) { m_name = _name; }
    void setTransparencyValue(const double _transparency_value) { m_transparency_value = _transparency_value; }

    void setOffsetX(double _offsetX) { m_offsetX = _offsetX; }
    void setOffsetY(double _offsetY){ m_offsetY = _offsetY; }
    void setOffsetZ(double _offsetZ){ m_offsetZ = _offsetZ; }

    void setLoadingMode(LoadingMode _loadingMode){ m_loadingMode = _loadingMode; }

    bool getUseExistingLOD() const;
    void setUseExistingLOD(bool useExistingLOD);

    bool getBuildLOD() const;
    void setBuildLOD(bool buildLOD);

    bool getSaveCompLOD() const;
    void setSaveCompLOD(bool saveCompLOD);

signals :
    void signal_createNode(osg::Node*, QString, QString _name, TdmLayerItem*, bool
                           ,double _transp, double _offsetX, double _offsetY, double offsetZ);

private:
    OSGWidget *m_osg_widget;
    QString m_filename;
    TdmLayerItem *m_parent;
    bool m_select_item;
    osg::ref_ptr<osg::Node> m_node;

    QString m_name;

    double m_transparency_value;

    double m_offsetX;
    double m_offsetY;
    double m_offsetZ;

    LoadingMode m_loadingMode;

    // LOD processing
    bool m_useExistingLOD;
    bool m_buildLOD;
    bool m_saveCompLOD;
};

#endif // FILE_OPEN_THREAD_H
