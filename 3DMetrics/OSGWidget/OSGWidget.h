#ifndef OSGWidget_h__
#define OSGWidget_h__

#include <QPoint>
#include <QtOpenGL>
#include <QTimer>
#include <QFileDialog>
#include <QPointF>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>
#include "line_measurement_tool.h"
#include "surface_measurement_tool.h"
#include "interest_point_tool.h"
#include "kml_handler.h"
#include <GeographicLib/LocalCartesian.hpp>

#define INVALID_VALUE 100000



class OSGWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    OSGWidget( QWidget* parent = 0);

    virtual ~OSGWidget();

    ///
    /// \brief setSceneFromFile load a scene from a 3D file
    /// \param _sceneFile path to any 3D file supported by osg
    /// \return true if loading succeded
    ///
    bool setSceneFromFile(std::string _sceneFile);

    ///
    /// \brief setSceneData load a scene from a osg::ref_ptr<osg::Node>
    /// \param _sceneData pointer to scene data
    /// \return true if loading succeded
    ///
    bool setSceneData(osg::ref_ptr<osg::Node> _sceneData);

    ///
    /// \brief setClearColor set the clear color for all cameras
    /// \param _r red [0..1]
    /// \param _g green [0..1]
    /// \param _b blue [0..1]
    /// \param _alpha transparency [0..1]
    ///
    void setClearColor(double _r, double _g, double _b, double _alpha=1.0);

    ///
    /// \brief clearSceneData removes scene data
    ///
    void clearSceneData();

    ///
    /// \brief getIntersectionPoint ray trace (x,y) point on display to 3D point
    /// \param _x x coord on display
    /// \param _y y coord on display
    /// \param _inter_point 3D intersection point on scene
    /// \param _inter_exists true if intersection exists
    ///
    void getIntersectionPoint(int _x, int _y, osg::Vec3d &_inter_point, bool &_inter_exists);

    ///
    /// \brief getMeasurementGeode get measurement geode for tools to draw in it
    /// \return measurement geode
    ///
    osg::ref_ptr<osg::Geode> getMeasurementGeode();

    ///
    /// \brief forceGeodeUpdate force geode data to redraw
    ///
    void forceGeodeUpdate();

    ///
    /// \brief getGeoOrigin get geographic origin
    /// \param _ref_lat_lon lat lon origin
    /// \param _ref_depth depth origin
    ///
    void getGeoOrigin(QPointF &_ref_lat_lon, double &_ref_depth);

public slots:


signals:
    void sig_showMeasurementSavingPopup(double _norm, QString _measurement_type, int _measurement_index);
    void sig_onMousePress(Qt::MouseButton _button, int _x, int _y);

protected:

    virtual void paintGL();
    virtual void resizeGL( int width, int height );

    virtual void keyPressEvent( QKeyEvent* event );
    virtual void keyReleaseEvent( QKeyEvent* event );

    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void wheelEvent( QWheelEvent* event );

    virtual bool event( QEvent* event );

    virtual void initializeGL();
    QTimer m_timer;


private:

    virtual void onResize( int width, int height );

    osgGA::EventQueue* getEventQueue() const;

    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> m_graphicsWindow;
    osg::ref_ptr<osgViewer::CompositeViewer> m_viewer;
    osg::Vec3d m_intersection_point;

    osgUtil::LineSegmentIntersector::Intersections::iterator m_hitr;

    std::vector<osg::ref_ptr<osg::Node>> m_models;
    osg::ref_ptr<osg::Group> m_group;
    osg::ref_ptr<osg::Geode> m_measurement_geode;

    // Georef objects
    KMLHandler m_kml_handler;
    QPointF m_ref_lat_lon;
    double m_ref_depth;
    GeographicLib::LocalCartesian m_ltp_proj;


};

#endif
