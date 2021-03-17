#ifndef SNAPGEOTIFFIMAGE_H
#define SNAPGEOTIFFIMAGE_H

#include <osg/Camera>
#include <QPointF>
#include <QWidget>

struct SnapGeotiffImage : public osg::Camera::DrawCallback
{
public:
    ///
    /// \brief SnapGeotiffImage
    /// \param _filename
    /// \param _ref_lat_lon
    /// \param _box
    /// \param _pixel_size
    /// \param _parentWidget
    ///
    SnapGeotiffImage(const std::string& _filename, QPointF &_ref_lat_lon, osg::BoundingBox _box, double _pixel_size, QWidget *_parentWidget);

    virtual void operator () (osg::RenderInfo& renderInfo) const;

    bool status() const { return m_status; }

    static bool process(osg::ref_ptr<osg::Node> _node, const std::string& _filename, QPointF &_ref_lat_lon, double _pixel_size, QWidget *_parentWidget);

private:
    std::string m_filename;
    osg::ref_ptr<osg::Image> m_image;
    QPointF m_ref_lat_lon;
    osg::BoundingBox m_box;
    double m_pixel_size;
    QWidget *m_parentWidget;
    bool m_status;
};

#endif // SNAPGEOTIFFIMAGE_H
