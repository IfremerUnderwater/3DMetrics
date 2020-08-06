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
    /// \brief loadFile
    /// \param _scene_file
    /// \param _mode
    /// \param _local_lat_lon
    /// \param _local_alt
    /// \return
    ///
    osg::ref_ptr<osg::Group>  loadFile(std::string _scene_file, LoadingMode _mode, QPointF &_local_lat_lon, double &_local_alt);

    ///
    /// \brief loadFile
    /// \param _scene_file
    /// \param _local_lat_lon
    /// \param _local_alt
    /// \param _lod
    /// \return
    ///
    osg::ref_ptr<osg::Group>  loadFileAndBuildTiles(std::string _scene_file, QPointF &_local_lat_lon, double &_local_alt, bool _lod = false);

    ///
    /// \brief loadTiles
    /// \param _scene_file
    /// \param _subdir
    /// \return
    ///
    osg::ref_ptr<osg::Group> loadTiles(std::string _scene_file, std::string _subdir = "");

};

#endif // GRIDFILEPROCESSOR_H
