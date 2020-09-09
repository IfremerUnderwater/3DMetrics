#include "file_open_thread.h"
#include <QtCore>

#include "OSGWidget/osg_widget.h"
#include "OSGWidget/grid_file_processor.h"
#include "osgDB/WriteFile"

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
    m_loadingMode(LoadingModeDefault),
    m_threshold1(0),
    m_threshold2(0),
    m_nTilesX(1),
    m_nTilesY(1)
{

}

void FileOpenThread::run()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    std::string pathToFile = m_filename.toStdString();
    std::string extension = "";
    int idx = pathToFile.rfind('.');
    if(idx != std::string::npos)
    {
        extension = pathToFile.substr(idx+1);
    }

    if(extension == "kml")
    {
        // kml
        KMLHandler kh;
        kh.readFile(pathToFile);

        pathToFile = kh.getModelPath();

        // check relative path
        if(pathToFile.size() > 0 && (!(pathToFile[0] == '/')))
        {
            std::string base_directory, lfname;
            kmlbase::File::SplitFilePath(m_filename.toStdString(),
                                         &base_directory,
                                         &lfname);
            pathToFile = base_directory + string(DIRSEP) + pathToFile;
        }

        idx = pathToFile.rfind('.');
        if(idx != std::string::npos)
        {
            extension = pathToFile.substr(idx+1);
        }
        else
        {
            extension = "";
        }

    }

    // check grd extension
    if(extension == "grd")
    {
        // GDAL (grid) processing
        m_node = m_osg_widget->createNodeFromFileWithGDAL(pathToFile, m_loadingMode, m_tileFolderName.toStdString());
    }
    else
    {
        GridFileProcessor gfp;

        switch(m_loadingMode)
        {
        case LoadingModeUseOSGB:
            m_node = m_osg_widget->createNodeFromFile(pathToFile + ".osgb");
            break;

        case LoadingModeBuildOSGB:
            m_node = m_osg_widget->createNodeFromFile(pathToFile);
            osgDB::writeNodeFile(*m_node,
                                 pathToFile + ".osgb",
                                 new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib"));
            break;

        case LoadingModeBuildAndUseSmartLOD:
            m_node = m_osg_widget->createNodeFromFile(pathToFile);
            m_osg_widget->createLODFiles(m_node, pathToFile, m_saveCompLOD);
            m_node = m_osg_widget->createLODNodeFromFiles(pathToFile);
            break;

        case LoadingModeUseSmartLOD:
            m_node = m_osg_widget->createLODNodeFromFiles(pathToFile);
            break;

        case LoadingModeLODTiles:
            m_node = gfp.loadTiles(pathToFile);
            break;

        case LoadingModeLODTilesDir:
            m_node = gfp.loadTiles(pathToFile, m_tileFolderName.toStdString());
            break;

        case LoadingModeSmartLODTiles:
            m_node = gfp.loadSmartLODTiles(pathToFile,"", m_threshold1, m_threshold2);
            break;

        case LoadingModeSmartLODTilesDir:
            m_node = gfp.loadSmartLODTiles(pathToFile, m_tileFolderName.toStdString(), m_threshold1, m_threshold2);
            break;


        case LoadingModeBuildLODTiles:
            m_node = m_osg_widget->createNodeFromFile(pathToFile);
            gfp.createLODTilesFromNodeGlobalSimplify(m_node,pathToFile,m_nTilesX,m_nTilesY,m_saveCompLOD,m_threshold1, m_threshold2);
            m_node = gfp.loadSmartLODTiles(pathToFile,"", m_threshold1, m_threshold2);
            break;

        default:
            // default processing
            m_node = m_osg_widget->createNodeFromFile(pathToFile);
        }
    }

    //        // LOD processing
    //        if(m_buildLOD)
    //        {
    //            // build LOD
    //            m_node = m_osg_widget->createNodeFromFile(m_filename.toStdString());
    //            std::string filename = m_filename.toStdString();

    //            if(filename.find_last_of(".kml") == filename.size()-1)
    //            {
    //                // kml

    //                KMLHandler kh;
    //                kh.readFile(filename);

    //                std::string pathToLodFile = kh.getModelPath();


    //                // check relative path
    //                if(pathToLodFile.size() > 0 && (!(pathToLodFile[0] == '/')))
    //                {
    //                    std::string base_directory,base_filename;
    //                    kmlbase::File::SplitFilePath(filename,
    //                                                 &base_directory,
    //                                                 &base_filename);
    //                    pathToLodFile = base_directory + string(DIRSEP) + pathToLodFile;

    //                    filename = pathToLodFile;
    //                }

    //            }

    //            m_osg_widget->createLODFiles(m_node, filename, m_saveCompLOD);

    //            m_useExistingLOD = true;
    //        }

    //        if(m_useExistingLOD)
    //        {
    //            //load existing LOD
    //            m_node = m_osg_widget->createLODNodeFromFiles(m_filename.toStdString());
    //        }
    //        else
    //        {
    //            // default processing
    //            m_node = m_osg_widget->createNodeFromFile(m_filename.toStdString());
    //        }
    //    }

    // get relative directory
    QDir dir( QFileInfo(m_filename).absoluteDir());
    QString relItemsDir = dir.relativeFilePath(m_tileFolderName);

    QApplication::restoreOverrideCursor();
    emit signal_createNode(m_node.get(),m_filename,m_name, m_parent,m_select_item, m_transparency_value, m_offsetX, m_offsetY, m_offsetZ,
                           m_threshold1, m_threshold2, m_loadingMode, relItemsDir);

}
//bool FileOpenThread::getUseExistingLOD() const
//{
//    return m_useExistingLOD;
//}

//void FileOpenThread::setUseExistingLOD(bool useExistingLOD)
//{
//    m_useExistingLOD = useExistingLOD;
//}
//bool FileOpenThread::getBuildLOD() const
//{
//    return m_buildLOD;
//}

//void FileOpenThread::setBuildLOD(bool buildLOD)
//{
//    m_buildLOD = buildLOD;
//}
//bool FileOpenThread::getSaveCompLOD() const
//{
//    return m_saveCompLOD;
//}

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
int FileOpenThread::getNTilesX() const
{
    return m_nTilesX;
}

void FileOpenThread::setNTilesX(int nTilesX)
{
    m_nTilesX = nTilesX;
}
int FileOpenThread::getNTilesY() const
{
    return m_nTilesY;
}

void FileOpenThread::setNTilesY(int nTilesY)
{
    m_nTilesY = nTilesY;
}








