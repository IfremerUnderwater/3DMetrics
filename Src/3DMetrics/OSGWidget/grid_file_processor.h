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
};

#endif // GRIDFILEPROCESSOR_H
