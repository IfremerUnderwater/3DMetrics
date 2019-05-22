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
#include <GeographicLib/LocalCartesian.hpp>

#include "kml_handler.h"


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
    bool setSceneFromFile(std::string _scene_file);

    ///
    /// \brief createNodeFromFile load a scene from a 3D file
    /// \param _sceneFile path to any 3D file supported by osg
    /// \return node if loading succeded
    ///
    osg::ref_ptr<osg::Node> createNodeFromFile(std::string _scene_file);

    ///
    /// \brief addNodeToScene add a node to the scene
    /// \param _node node to be added
    /// \return true if loading succeded
    ///
    bool addNodeToScene(osg::ref_ptr<osg::Node> _node);

    ///
    /// \brief removeNodeFromScene remove a node from the scene
    /// \param _node node to be removed
    /// \return true if remove succeded
    ///
    bool removeNodeFromScene(osg::ref_ptr<osg::Node> _node);

    ///
    /// \brief setSceneData load a scene from a osg::ref_ptr<osg::Node>
    /// \param _sceneData pointer to scene data
    /// \return true if loading succeded
    ///
    //bool setSceneData(osg::ref_ptr<osg::Node> _sceneData);

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

    //    ///
    //    /// \brief getMeasurementGeode get measurement geode for tools to draw in it
    //    /// \return measurement geode
    //    ///
    //    osg::ref_ptr<osg::Geode> getMeasurementGeode();

    //    ///
    //    /// \brief forceGeodeUpdate force geode data to redraw
    //    ///
    //    void forceGeodeUpdate();
    void addGeode(osg::ref_ptr<osg::Geode> _geode);
    void removeGeode(osg::ref_ptr<osg::Geode> _geode);

    // for measures
    void addGroup(osg::ref_ptr<osg::Group> _group);
    void removeGroup(osg::ref_ptr<osg::Group> _group);

    ///
    /// \brief getGeoOrigin get geographic origin
    /// \param _ref_lat_lon lat lon origin
    /// \param _ref_depth depth origin
    ///
    void getGeoOrigin(QPointF &_ref_lat_lon, double &_ref_depth);

    // set initial values
    void setGeoOrigin(QPointF _latlon, double _depth);

    // reset to home position
    void home();

    // convert x, y, z => lat, lon & depth
    // if(m_ref_depth == INVALID_VALUE) do nothing
    void xyzToLatLonDepth(double _x, double _y, double _z, double &_lat, double &_lon, double &_depth);

public slots:

signals:
    void sig_showMeasurementSavingPopup(double _norm, QString _measurement_type, int _measurement_index);
    void signal_onMousePress(Qt::MouseButton _button, int _x, int _y);
    void signal_onMouseMove(int _x, int _y);

    // tools
    void signal_startTool(QString &message);
    void signal_endTool(QString &message);
    void signal_cancelTool(QString &message);

public:
    // tools : emit correspondant signal
    void startTool(QString &message);
    void endTool(QString &message);
    void cancelTool(QString &message);

    // screen2D
    bool generateOrthoMap(osg::ref_ptr<osg::Node> _node, QString _filename, osg::BoundingBox _box, double _dPixel);

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
    //osg::Vec3d m_intersection_point;

    //osgUtil::LineSegmentIntersector::Intersections::iterator m_hitr;

    osg::ref_ptr<osg::Group> m_group;

    std::vector<osg::ref_ptr<osg::Geode>> m_geodes;
    std::vector<osg::ref_ptr<osg::Node>> m_models;

    // not used anymore
    //std::vector<osg::ref_ptr<osg::Group>> m_groups;
    //osg::ref_ptr<osg::Geode> m_measurement_geode;

    // Georef objects
    KMLHandler m_kml_handler;
    QPointF m_ref_lat_lon;
    double m_ref_depth;
    GeographicLib::LocalCartesian m_ltp_proj;


};

#endif
