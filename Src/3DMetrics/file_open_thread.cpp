#include "file_open_thread.h"
#include <QtCore>

#include "OSGWidget/osg_widget.h"

FileOpenThread::FileOpenThread() :
    m_select_item(false),
    m_transparency_value(0),
    m_offsetX(0),
    m_offsetY(0),
    m_offsetZ(0),
    m_loadingMode(LoadingModePoint)
{

}

void FileOpenThread::run()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    // check grd extension
    if(m_filename.toStdString().find_last_of(".grd") == m_filename.toStdString().size()-1)
    {
        m_node = m_osg_widget->createNodeFromFileWithGDAL(m_filename.toStdString(), m_loadingMode);
    }
    else
    {
        m_node = m_osg_widget->createNodeFromFile(m_filename.toStdString());
    }

    QApplication::restoreOverrideCursor();
    emit signal_createNode(m_node.get(),m_filename,m_name, m_parent,m_select_item, m_transparency_value, m_offsetX, m_offsetY, m_offsetZ);

}
