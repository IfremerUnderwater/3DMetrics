#include "mythreadcreatenode.h"
#include <QtCore>

#include "OSGWidget/osg_widget.h"

MyThreadCreateNode::MyThreadCreateNode()
{

}

void MyThreadCreateNode::run()
{
    QMutex mutex;
    mutex.lock();
   /* if(this->Stop)
    {
        break;
    }*/
    mutex.unlock();

    QApplication::setOverrideCursor(Qt::WaitCursor);
     m_node = m_osgwidget->createNodeFromFile(m_filename.toStdString());

    osg::Node* node = NULL;
    node = m_node.get();
     m_node.release();
     QApplication::restoreOverrideCursor();
    emit signal_createNode(node,m_filename,m_parent,m_selectItem);

}
