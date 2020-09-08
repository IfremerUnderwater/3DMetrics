#include "file_open_thread.h"
#include <QtCore>

#include "OSGWidget/osg_widget.h"

#if defined(_WIN32) || defined(WIN32)
#define DIRSEP "\\"
#else
#define DIRSEP "/"
#endif

FileOpenThread::FileOpenThread() :
    m_select_item(false),
    m_transparency_value(0),
    m_offsetX(0),
    m_offsetY(0),
    m_offsetZ(0),
    m_loadingMode(LoadingModePoint),
    m_threshold1(0),
    m_threshold2(0)
{

}

void FileOpenThread::run()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    // check grd extension
    if(m_filename.toStdString().find_last_of(".grd") == m_filename.toStdString().size()-1)
    {
        // GDAL (grid) processing
        m_node = m_osg_widget->createNodeFromFileWithGDAL(m_filename.toStdString(), m_loadingMode, m_tileFolderName.toStdString());
    }
    else
    {
        // LOD processing
        if(m_buildLOD)
        {
            // build LOD
            m_node = m_osg_widget->createNodeFromFile(m_filename.toStdString());
            std::string filename = m_filename.toStdString();

            if(filename.find_last_of(".kml") == filename.size()-1)
            {
                // kml

                KMLHandler kh;
                kh.readFile(filename);

                std::string pathToLodFile = kh.getModelPath();


                // check relative path
                if(pathToLodFile.size() > 0 && (!(pathToLodFile[0] == '/')))
                {
                    std::string base_directory,base_filename;
                    kmlbase::File::SplitFilePath(filename,
                                                 &base_directory,
                                                 &base_filename);
                    pathToLodFile = base_directory + string(DIRSEP) + pathToLodFile;

                    filename = pathToLodFile;
                }

            }

            m_osg_widget->createLODFiles(m_node, filename, m_saveCompLOD);

            m_useExistingLOD = true;
        }

        if(m_useExistingLOD)
        {
            //load existing LOD
            m_node = m_osg_widget->createLODNodeFromFiles(m_filename.toStdString());
        }
        else
        {
            // default processing
            m_node = m_osg_widget->createNodeFromFile(m_filename.toStdString());
        }
    }

    // get relative directory
    QDir dir( QFileInfo(m_filename).absoluteDir());
    QString relItemsDir = dir.relativeFilePath(m_tileFolderName);

    QApplication::restoreOverrideCursor();
    emit signal_createNode(m_node.get(),m_filename,m_name, m_parent,m_select_item, m_transparency_value, m_offsetX, m_offsetY, m_offsetZ,
                           m_threshold1, m_threshold2, m_loadingMode, relItemsDir);

}
bool FileOpenThread::getUseExistingLOD() const
{
    return m_useExistingLOD;
}

void FileOpenThread::setUseExistingLOD(bool useExistingLOD)
{
    m_useExistingLOD = useExistingLOD;
}
bool FileOpenThread::getBuildLOD() const
{
    return m_buildLOD;
}

void FileOpenThread::setBuildLOD(bool buildLOD)
{
    m_buildLOD = buildLOD;
}
bool FileOpenThread::getSaveCompLOD() const
{
    return m_saveCompLOD;
}

void FileOpenThread::setSaveCompLOD(bool saveCompLOD)
{
    m_saveCompLOD = saveCompLOD;
}
QString FileOpenThread::getTileFolderName() const
{
    return m_tileFolderName;
}

void FileOpenThread::setTileFolderName(const QString &tileFolderName)
{
    m_tileFolderName = tileFolderName;
}
float FileOpenThread::getThreshold1() const
{
    return m_threshold1;
}

void FileOpenThread::setThreshold1(float threshold1)
{
    m_threshold1 = threshold1;
}
float FileOpenThread::getThreshold2() const
{
    return m_threshold2;
}

void FileOpenThread::setThreshold2(float threshold2)
{
    m_threshold2 = threshold2;
}






