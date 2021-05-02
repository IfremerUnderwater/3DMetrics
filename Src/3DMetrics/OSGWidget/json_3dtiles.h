#ifndef JSON3DTILES_H
#define JSON3DTILES_H

#include <string>

#include <osg/Node>
#include <osg/BoundingBox>

class QJsonArray;
class QJsonObject;
class QString;

///
/// \brief The Json3dTiles class
///
/// All coordinates in meters
///
class Json3dTiles
{
public:
    Json3dTiles();

    void setRootNode(osg::ref_ptr<osg::Node> _rootNode, std::string _rootFileName);

    void addRootLODFiles(std::string _baseName, double _fullGeometricError, double _mediumGeometricError, double _rawGeometricError);

    void addRootLODTilesFiles(std::string _baseName, int _nTileX, int _nTileY, double _fullGeometricError, double _mediumGeometricError, double _rawGeometricError);
    void addRootTilesFiles(std::string _baseName, int _nTileX, int _nTileY, double _geometricError);

    bool writeFile(std::string _fileName);

    double rootGeometricError() const;
    void setRootGeometricError(double _rootGeometricError);

    double globalGeometricError() const;
    void setGlobalGeometricError(double _globalGeometricError);

    bool useRelativeFilename() const;
    void setUseRelativeFilename(bool _useRelativeFilename);

private:
    QJsonArray buildBoundingVolumeBox(osg::BoundingBox _box);
    QJsonObject buildLOD(osg::BoundingBox _box, std::string _baseFileName);
    osg::BoundingBox buildTileBox(osg::BoundingBox &_box, const int _x, const int _y);

    // global to file
    double m_globalGeometricError;

    osg::ref_ptr<osg::Node> m_rootNode;
    std::string m_rootFilename;
    // for root node
    double m_rootGeometricError;

    // LOD
    std::string m_baseName;
    double m_fullGeometricError;
    double m_mediumGeometricError;
    double m_rawGeometricError;

    // Tiles (0 if no tiles)
    int m_nTileX;
    int m_nTileY;

    // for coumpoud lod tiles (or ordinany tiles - not LOD)
    bool m_simpleTiles;

    // relative filename
    bool m_useRelativeFilename;

    QString fileUri(std::string _fileName);
};

#endif // JSON3DTILES_H
