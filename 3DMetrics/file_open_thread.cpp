#include "file_open_thread.h"
#include <QtCore>

#include "OSGWidget/osg_widget.h"

FileOpenThread::FileOpenThread()
{

}

void FileOpenThread::run()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
     m_node = m_osg_widget->createNodeFromFile(m_filename.toStdString());

    osg::Node* node = NULL;
    node = m_node.get();
     m_node.release();
     QApplication::restoreOverrideCursor();
    emit signal_createNode(node,m_filename,m_parent,m_select_item);

}
