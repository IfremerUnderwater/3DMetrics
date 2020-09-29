#ifndef ELEVATIONMAPCREATOR_H
#define ELEVATIONMAPCREATOR_H

#include <QPointF>
#include <QWidget>

#include <osg/BoundingBox>
#include <osgViewer/Viewer>

class ElevationMapCreator
{
public:

    ///
    /// \brief ElevationMapCreator
    /// \param _filename
    /// \param _ref_lat_lon
    /// \param _box
    /// \param _pixel_size
    /// \param _width_pixel
    /// \param _height_pixel
    ///
    ElevationMapCreator(const std::string& _filename, QPointF &_ref_lat_lon,
                        osg::BoundingBox _box, double _pixel_size,
                        int _width_pixel, int _height_pixel);

    ///
    /// \brief process
    /// \param _viewer
    /// \param _parentWidget
    /// \return
    ///
    bool process(osgViewer::Viewer &_viewer, QWidget *_parentWidget);

private:
    std::string m_filename;
    QPointF m_ref_lat_lon;
    double m_pixel_size;
    osg::BoundingBox m_box;
    int m_width_pixel;
    int m_height_pixel;
};

#endif // ELEVATIONMAPCREATOR_H
