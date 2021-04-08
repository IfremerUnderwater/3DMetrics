#include "file_open_thread.h"
#include <QtCore>

#include "OSGWidget/osg_widget.h"
#include "OSGWidget/grid_file_processor.h"
#include "OSGWidget/lod_tools.h"

#include "osgDB/WriteFile"

#include "OSGWidget/json_3dtiles.h"

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
    m_nTilesY(1),
    m_inProjectOpen(false)
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
            m_node = m_osg_widget->createNodeFromFile(pathToFile, LoadingModeUseOSGB);
            break;

        case LoadingModeBuildOSGB:
            m_node = m_osg_widget->createNodeFromFile(pathToFile, LoadingModeBuildOSGB);
            break;

        case LoadingModeBuildAndUseSmartLOD:
            m_node = m_osg_widget->createNodeFromFile(pathToFile);
            m_osg_widget->createLODFiles(m_node, pathToFile, m_saveCompLOD);
            m_node = m_osg_widget->createLODNodeFromFiles(pathToFile);
            if(true)
            {
                // 3dTiles file
                if(m_saveCompLOD)
                {
                    Json3dTiles json;
                    json.setRootNode(m_node, pathToFile + ".osgb");
                    json.writeFile(pathToFile + "-compound.json");
                }

                Json3dTiles json;

                json.setRootNode(m_node, "");
                json.addRootLODFiles(pathToFile, 0, 0.1, 1.0);
                json.writeFile(pathToFile + ".json");
            }
            break;

        case LoadingModeUseSmartLOD:
            m_node = m_osg_widget->createLODNodeFromFiles(pathToFile);
            break;

        case LoadingModeLODTiles:
            m_node = m_osg_widget->createNodeFromFile(pathToFile, LoadingModeLODTiles);
            break;

        case LoadingModeLODTilesDir:
            m_node = m_osg_widget->createNodeFromFile(pathToFile, LoadingModeLODTiles,  m_tileFolderName.toStdString());
            break;

        case LoadingModeSmartLODTiles:
            m_node = m_osg_widget->createNodeFromFile(pathToFile, LoadingModeSmartLODTiles);
            LODTools::applyLODValuesInTree(m_node, m_threshold1, m_threshold2);
            break;

        case LoadingModeSmartLODTilesDir:
            m_node = m_osg_widget->createNodeFromFile(pathToFile, LoadingModeSmartLODTilesDir,  m_tileFolderName.toStdString());
            LODTools::applyLODValuesInTree(m_node, m_threshold1, m_threshold2);
            break;

        case LoadingModeBuildLODTiles:
            m_node = m_osg_widget->createNodeFromFile(pathToFile);
            // KML processing
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
            }
            gfp.createLODTilesFromNodeGlobalSimplify(m_node,pathToFile,m_nTilesX,m_nTilesY,m_saveCompLOD,m_threshold1, m_threshold2);
            m_node = m_osg_widget->createNodeFromFile(m_filename.toStdString(), LoadingModeSmartLODTiles);
            LODTools::applyLODValuesInTree(m_node, m_threshold1, m_threshold2);
            if(true)
            {
                // 3dTiles file
                if(m_saveCompLOD)
                {
                    Json3dTiles json;
                    json.setRootNode(m_node, m_filename.toStdString());
                    json.addRootTilesFiles(pathToFile, m_nTilesX, m_nTilesY, 0);
                    json.writeFile(pathToFile + "-compound.json");
                }

                Json3dTiles json;

                json.setRootNode(m_node, m_filename.toStdString());
                json.addRootLODTilesFiles(pathToFile, m_nTilesX, m_nTilesY, 0, 0.1, 1.0);
                json.writeFile(pathToFile + ".json");
            }
            break;

        case LoadingModeBuildTiles:
            m_node = m_osg_widget->createNodeFromFile(pathToFile);
            // KML processing
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
            }
            gfp.createTilesFromNode(m_node,pathToFile,m_nTilesX,m_nTilesY);
            m_node = m_osg_widget->createNodeFromFile(m_filename.toStdString(), LoadingModeLODTiles);
            if(true)
            {
                Json3dTiles json;

                json.setRootNode(m_node, "");
                json.addRootTilesFiles(pathToFile, m_nTilesX, m_nTilesY, 0);
                json.writeFile(pathToFile + ".json");
            }
            break;

        default:
            // default processing
            m_node = m_osg_widget->createNodeFromFile(pathToFile);
        }
    }


    // get relative directory
    QDir dir( QFileInfo(m_filename).absoluteDir());
    QString relItemsDir = dir.relativeFilePath(m_tileFolderName);

    QApplication::restoreOverrideCursor();
    emit signal_createNode(m_node.get(),m_filename,m_name, m_parent,m_select_item, m_transparency_value, m_offsetX, m_offsetY, m_offsetZ,
                           m_threshold1, m_threshold2, m_loadingMode, relItemsDir, m_inProjectOpen);

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

bool FileOpenThread::getInProjectOpen() const
{
    return m_inProjectOpen;
}

void FileOpenThread::setInProjectOpen(bool inProjectOpen)
{
    m_inProjectOpen = inProjectOpen;
}
