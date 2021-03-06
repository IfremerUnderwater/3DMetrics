#ifndef GRIDFILEPROCESSOR_H
#define GRIDFILEPROCESSOR_H

#include <QPointF>
#include <osg/Group>
#include "loading_mode.h"

class GridFileProcessor
{
public:
    GridFileProcessor();
    virtual ~GridFileProcessor();

    ///
    /// \brief loadGridFile
    /// \param _scene_file
    /// \param _mode
    /// \param _local_lat_lon
    /// \param _local_alt
    /// \return
    ///
    osg::ref_ptr<osg::Group>  loadGridFile(std::string _scene_file, LoadingMode _mode, QPointF &_local_lat_lon, double &_local_alt);

    ///
    /// \brief getGridLatLonAlt
    /// \param _scene_file
    /// \param _local_lat_lon
    /// \param _local_alt
    ///
    void getGridLatLonAlt(std::string _scene_file, QPointF &_local_lat_lon, double &_local_alt);

    ///
    /// \brief loadGridFileAndBuildTiles
    /// \param _scene_file
    /// \param _local_lat_lon
    /// \param _local_alt
    /// \param _lod
    /// \return
    ///
    osg::ref_ptr<osg::Group>  loadGridFileAndBuildTiles(std::string _scene_file, QPointF &_local_lat_lon, double &_local_alt, bool _lod = false);

    ///
    /// \brief loadTiles
    /// \param _scene_file
    /// \param _subdir
    /// \return
    ///
    osg::ref_ptr<osg::Group> loadTiles(std::string _scene_file, std::string _subdir = "");

    ///
    /// \brief loadSmartLODTiles
    /// \param _scene_file
    /// \param _subdir
    /// \param _threshold1
    /// \param _threshold2
    /// \return
    ///
    osg::ref_ptr<osg::Group> loadSmartLODTiles(std::string _scene_file, std::string _subdir = "", float _threshold1 = 800.0f, float _threshold2 = 2500.0f);

    ///
    /// \brief createLODTilesFromNode
    /// \param _node
    /// \param _scene_file_basename
    /// \param _nTilesX
    /// \param _nTilesY
    /// \param _buildCompoundLOD
    /// \param _threshold1
    /// \param _threshold2
    /// \return
    ///
    bool createLODTilesFromNode(osg::ref_ptr<osg::Node> _node, std::string _scene_file_basename, int _nTilesX, int _nTilesY, bool _buildCompoundLOD, float _threshold1 = 40.0f, float _threshold2 = 200.0f);

    ///
    /// \brief createTilesFromNode
    /// \param _node
    /// \param _scene_file_basename
    /// \param _nTilesX
    /// \param _nTilesY
    /// \return
    ///
    bool createTilesFromNode(osg::ref_ptr<osg::Node> _node, std::string _scene_file_basename, int _nTilesX, int _nTilesY);

    ///
    /// \brief createLODTilesFromNodeGlobalSimplify
    /// \param _node
    /// \param _scene_file_basename
    /// \param _nTilesX
    /// \param _nTilesY
    /// \param _buildCompoundLOD
    /// \param _threshold1
    /// \param _threshold2
    /// \return
    ///
    bool createLODTilesFromNodeGlobalSimplify(osg::ref_ptr<osg::Node> _node, std::string _scene_file_basename, int _nTilesX, int _nTilesY, bool _buildCompoundLOD, float _threshold1 = 40.0f, float _threshold2 = 200.0f);

};

#endif // GRIDFILEPROCESSOR_H
