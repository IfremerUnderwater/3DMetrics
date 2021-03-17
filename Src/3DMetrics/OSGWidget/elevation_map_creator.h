#ifndef ELEVATIONMAPCREATOR_H
#define ELEVATIONMAPCREATOR_H

#include <QPointF>
#include <QWidget>

#include <osg/BoundingBox>
#include <osgViewer/Viewer>
#include <osg/Node>

class ElevationMapCreator
{
public:
    ///
    /// \brief ElevationMapCreator
    /// \param _ref_lat_lon
    /// \param _pixel_size
    ///
    ElevationMapCreator(QPointF &_ref_lat_lon, double _pixel_size);

    ///
    /// \brief process
    /// \param _node
    /// \param _filename
    /// \param _parentWidget
    /// \return
    ///
    bool process(osg::ref_ptr<osg::Node> _node, const std::string& _filename,  QWidget *_parentWidget);

private:
    ///
    /// \brief processWrite
    /// \param _viewer
    /// \param _filename
    /// \param _parentWidget
    /// \return
    ///
    bool processWrite(osgViewer::View &_view, osg::BoundingBox &box, const std::string &_filename, QWidget *_parentWidget);

    QPointF m_ref_lat_lon;
    double m_pixel_size;
    int m_width_pixel;
    int m_height_pixel;
};

#endif // ELEVATIONMAPCREATOR_H
