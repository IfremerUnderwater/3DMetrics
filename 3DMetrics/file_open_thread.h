#ifndef FILE_OPEN_THREAD_H
#define FILE_OPEN_THREAD_H

#include <QThread>

#include <osg/Node>
#include "TreeView/tdm_layer_item.h"
class OSGWidget;

class FileOpenThread : public QThread
{
    Q_OBJECT
public:
    explicit FileOpenThread();
    void run();
    bool Stop;
    void setOSGWidget(OSGWidget *_osg_widget) {m_osg_widget = _osg_widget;}
    void setFileName(QString _filename) {m_filename = _filename;}
    void setTDMLayerItem(TdmLayerItem *_parent) {m_parent = _parent;}
    void setSelectItem(bool _select_item) {m_select_item = _select_item;}
    osg::ref_ptr<osg::Node> getNode() {return m_node;}

signals :
    void signal_createNode(osg::Node*,QString ,TdmLayerItem*,bool);

private:
    OSGWidget *m_osg_widget;
    QString m_filename;
    TdmLayerItem *m_parent;
    bool m_select_item;
    osg::ref_ptr<osg::Node> m_node;

};

#endif // FILE_OPEN_THREAD_H
