#include "json_3dtiles.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>

#include "box_visitor.h"
#include "smartlod.h"

#include <QDebug>

Json3dTiles::Json3dTiles()
{
    m_fullGeometricError = 0;
    m_mediumGeometricError = 0;
    m_rawGeometricError = 0;
    m_nTileX = 0;
    m_nTileY = 0;
    m_simpleTiles = false;
    m_useRelativeFilename = true;
}

void Json3dTiles::setRootNode(osg::ref_ptr<osg::Node> _rootNode, std::string _rootFileName)
{
    m_rootNode = _rootNode;
    m_rootFilename = _rootFileName;
    m_rootGeometricError = 1.0;
    // geometricerror : what to put here ?
    m_globalGeometricError = 500.0;
}

void Json3dTiles::addRootLODFiles(std::string _baseName, double _fullGeometricError, double _mediumGeometricError, double _rawGeometricError)
{
    m_baseName = _baseName;
    m_fullGeometricError = _fullGeometricError;
    m_mediumGeometricError = _mediumGeometricError;
    m_rawGeometricError = _rawGeometricError;
}

void Json3dTiles::addRootLODTilesFiles(std::string _baseName, int _nTileX, int _nTileY, double _fullGeometricError, double _mediumGeometricError, double _rawGeometricError)
{
    m_baseName = _baseName;
    m_fullGeometricError = _fullGeometricError;
    m_mediumGeometricError = _mediumGeometricError;
    m_rawGeometricError = _rawGeometricError;
    m_nTileX = _nTileX;
    m_nTileY = _nTileY;
    m_simpleTiles = false;
}

void Json3dTiles::addRootTilesFiles(std::string _baseName, int _nTileX, int _nTileY, double _geometricError)
{
    m_baseName = _baseName;
    m_fullGeometricError = _geometricError;
    m_mediumGeometricError = _geometricError;
    m_rawGeometricError = _geometricError;
    m_nTileX = _nTileX;
    m_nTileY = _nTileY;
    m_simpleTiles = true;
}

bool Json3dTiles::writeFile(std::string _fileName)
{
    if( m_rootNode == nullptr)
        return false;

    QJsonObject root;

    // asset
    QJsonObject asset;
    asset["version"] = "1.0";
    root["asset"] = asset;

    // geometricerror : what to put here ?
    root["geometricError"] = m_globalGeometricError;

    // rootTile
    QJsonObject rootTile;

    // TODO : use boundingVolume.region if kml (and grid ?)

    // box
    //    The boundingVolume.box property is an array of 12 numbers that define an oriented
    //    bounding box in a right-handed 3-axis (x, y, z) Cartesian coordinate system where the z -axis
    //    is up. The first three elements define the x, y, and z values for the center of the box. The next
    //    three elements (with indices 3, 4, and 5) define the x -axis direction and half-length. The next
    //    three elements (indices 6, 7, and 8) define the y -axis direction and half-length. The last three
    //    elements (indices 9, 10, and 11) define the z -axis direction and half-length.
    BoxVisitor bv;
    m_rootNode->accept(bv);
    osg::BoundingBox box = bv.getBoundingBox();

    QJsonArray boundingbox = buildBoundingVolumeBox(box);

    QJsonObject boxv;
    boxv["box"] = boundingbox;

    if(m_rootFilename.size() > 0)
    {
        // One file
        rootTile["boundingVolume"] = boxv;
        rootTile["geometricError"] = m_rootGeometricError;

        QJsonObject content;
        content["uri"] = fileUri(m_rootFilename);
        rootTile["content"] = content;
    }

    if(m_baseName.size() > 0 && m_nTileX == 0 && m_nTileY == 0)
    {
        // 3 LOD files
        rootTile = buildLOD(box, m_baseName);
    }
    else if (m_baseName.size() > 0 && m_nTileX > 0 && m_nTileY > 0)
    {
        // empty (no content) root tile
        // One file
        rootTile["boundingVolume"] = boxv;
        rootTile["geometricError"] = m_rootGeometricError;
        rootTile["refine"] = "ADD";

        // total bounding box
        BoxVisitor bv;
        m_rootNode->accept(bv);
        osg::BoundingBox box = bv.getBoundingBox();

        QJsonArray children;

        // process tiles
        for(int x=0; x<m_nTileX; x++)
        {
            for(int y=0; y<m_nTileY; y++)
            {
                osg::BoundingBox tileBox = buildTileBox(box,x,y);

                // build filename
                std::string name = m_rootFilename;
                // add tile number
                char buffer[80];
                sprintf(buffer, ".%03d_%03d", x, y);
                name = name + buffer;

                qDebug() << "FileNameBase=" << name.c_str();
                qDebug() << "BBox " << tileBox.xMin() << " " << tileBox.xMax() << " " << tileBox.yMin() << " " << tileBox.yMax();

                if(!m_simpleTiles)
                {
                    // check file existence before add
                    if(QFile::exists((name + SmartLOD::EXTLOD0).c_str())
                            && QFile::exists((name + SmartLOD::EXTLOD1).c_str())
                            && QFile::exists((name + SmartLOD::EXTLOD2).c_str()))
                    {
                        QJsonObject lod = buildLOD(tileBox, name);

                        // Add LOD to children
                        children.push_back(lod);
                    }
                }
                else
                {
                    // simple (not Smart LOD) tile
                    QString fname = (name + ".osgb").c_str();
                    if(QFile::exists(fname))
                    {
                        QJsonObject tile;
                        QJsonArray boundingbox = buildBoundingVolumeBox(tileBox);
                        // One file
                        tile["boundingVolume"] = boundingbox;
                        tile["geometricError"] = m_fullGeometricError;

                        QJsonObject content;
                        content["uri"] = fileUri(fname.toStdString());
                        tile["content"] = content;

                        children.push_back(tile);
                    }
                }
            }
        }
        rootTile["children"] = children;
    }
    root["root"] = rootTile;

    // save
    QFile file(_fileName.c_str());
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QJsonDocument json(root);
    QString json_string = json.toJson();
    file.write(json_string.toUtf8());
    file.close();

    return true;
}

QJsonArray Json3dTiles::buildBoundingVolumeBox(osg::BoundingBox _box)
{
    QJsonArray boundingbox;

    // center
    boundingbox.push_back(_box.center().x());
    boundingbox.push_back(_box.center().y());
    boundingbox.push_back(_box.center().z());

    // x-axis
    boundingbox.push_back(_box.xMax() - _box.center().x());
    boundingbox.push_back(0);
    boundingbox.push_back(0);

    // y-axis
    boundingbox.push_back(0);
    boundingbox.push_back(_box.yMax() - _box.center().y());
    boundingbox.push_back(0);

    // z-axis
    boundingbox.push_back(0);
    boundingbox.push_back(0);
    boundingbox.push_back(_box.zMax() - _box.center().z());

    return boundingbox;
}

QJsonObject Json3dTiles::buildLOD(osg::BoundingBox _box, std::string _baseFileName)
{
    QJsonObject tile;
    QJsonArray boundingbox = buildBoundingVolumeBox(_box);

    QJsonObject boxv;
    boxv["box"] = boundingbox;

    // 3 LOD files
    tile["boundingVolume"] = boxv;

    // LOD processing;
    tile["geometricError"] = m_rawGeometricError;
    QJsonObject content;
    content["uri"] = fileUri(_baseFileName + SmartLOD::EXTLOD2);
    tile["content"] = content;
    tile["refine"] = "REPLACE";

    // children
    QJsonArray children2;
    QJsonObject lod2;
    lod2["boundingVolume"] = boxv;
    QJsonObject content2;
    content2["uri"] = fileUri(_baseFileName + SmartLOD::EXTLOD0);
    lod2["content"] = content2;
    lod2["geometricError"] = m_fullGeometricError;
    children2.push_back(lod2);

    // children
    QJsonArray children1;
    QJsonObject lod1;
    lod1["boundingVolume"] = boxv;
    QJsonObject content1;
    content1["uri"] = fileUri(_baseFileName + SmartLOD::EXTLOD1);
    lod1["content"] = content1;
    lod1["geometricError"] = m_mediumGeometricError;
    lod1["children"] = children2;

    children1.push_back(lod1);

    tile["children"] = children1;

    return tile;
}

osg::BoundingBox Json3dTiles::buildTileBox(osg::BoundingBox &_box, const int _x, const int _y)
{
    osg::BoundingBox ret;

    if(_x >= 0 && _x < m_nTileX && _y >= 0 && _y < m_nTileY)
    {
        // x
        ret.xMin() = _box.xMin() + _x * (_box.xMax() - _box.xMin()) /m_nTileX;
        ret.xMax() = ret.xMin() + (_box.xMax() - _box.xMin()) /m_nTileX;

        // y
        ret.yMin() = _box.yMin() + _y * (_box.yMax() - _box.yMin()) /m_nTileY;
        ret.yMax() = ret.yMin() + (_box.yMax() - _box.yMin()) /m_nTileY;

        // z unchanged
        ret.zMin() = _box.zMin();
        ret.zMax() = _box.zMax();
    }
    return ret;
}

QString Json3dTiles::fileUri(std::string _fileName)
{
    if(m_useRelativeFilename)
    {
        QFileInfo info(_fileName.c_str());
        return info.fileName();
    }
    else
    {
        return _fileName.c_str();
    }
}

bool Json3dTiles::useRelativeFilename() const
{
    return m_useRelativeFilename;
}

void Json3dTiles::setUseRelativeFilename(bool _useRelativeFilename)
{
    m_useRelativeFilename = _useRelativeFilename;
}

double Json3dTiles::globalGeometricError() const
{
    return m_globalGeometricError;
}

void Json3dTiles::setGlobalGeometricError(double _globalGeometricError)
{
    m_globalGeometricError = _globalGeometricError;
}

double Json3dTiles::rootGeometricError() const
{
    return m_rootGeometricError;
}

void Json3dTiles::setRootGeometricError(double _rootGeometricError)
{
    m_rootGeometricError = _rootGeometricError;
}
