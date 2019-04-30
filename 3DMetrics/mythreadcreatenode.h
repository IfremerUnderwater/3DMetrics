#ifndef MYTHREADCREATENODE_H
#define MYTHREADCREATENODE_H

#include <QThread>

#include <osg/Node>
#include "TreeView/tdm_layer_item.h"
class OSGWidget;

class MyThreadCreateNode : public QThread
{
    Q_OBJECT
public:
    explicit MyThreadCreateNode();
    void run();
    bool Stop;
    void setOSGWidget(OSGWidget *_osg) {m_osgwidget = _osg;}
    void setFileName(QString _filename) {m_filename = _filename;}
    void setTDMLayerItem(TdmLayerItem *_parent) {m_parent = _parent;}
    void setSelectItem(bool _selectItem) {m_selectItem = _selectItem;}
    osg::ref_ptr<osg::Node> getNode() {return m_node;}

signals :
    void signal_createNode(osg::Node*,QString ,TdmLayerItem*,bool);

private:
    OSGWidget *m_osgwidget;
    QString m_filename;
    TdmLayerItem *m_parent;
    bool m_selectItem;
    osg::ref_ptr<osg::Node> m_node;

};

#endif // MYTHREADCREATENODE_H
