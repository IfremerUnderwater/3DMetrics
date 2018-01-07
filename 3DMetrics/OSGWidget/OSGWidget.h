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
#include "tool_handler.h"
#include <GeographicLib/LocalCartesian.hpp>

#define INVALID_VALUE -1000



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

    void removeLastMeasurementOfType(ToolState _meas_type);

    void removeMeasurementOfType(ToolState _meas_type, int _meas_index);

    // hide/show measurement method
    void hideShowMeasurementOfType(ToolState _meas_type, int _meas_index, bool _visible);

    void getIntersectionPoint(int _x, int _y, osg::Vec3d &_inter_point, bool &_inter_exists);


public slots:
    void slot_setInIdleState();
    void slot_setInLineMeasurementState();
    void slot_setInSurfaceMeasurementState();
    void slot_setInInterestPointState();
    void slot_setInCutAreaState();
    void slot_setInZoomInState();
    void slot_setInZoomOutState();
    void slot_setInFullScreenState();
    void slot_setInCropState();
    void sl_resetMeasur();

signals:
    void sig_showMeasurementSavingPopup(double _norm, QString _measurement_type, int _measurement_index);
    void si_showInterestPointMeasurementSavingPopup(QString _coordinates, QString _measurement_type, int _measurement_index);
    void si_endMeasur();
    void si_returnIdleState();


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

    // Measurements tools
    ToolState m_tool_state;
    LineMeasurementTool m_line_measurement_tool;
    SurfaceMeasurementTool m_surface_measurement_tool;
    InterestPointTool m_interest_point_tool;

    QWidget m_distance_meas_form_pop;

    // Georef objects
    KMLHandler m_kml_handler;
    QPointF m_ref_lat_lon;
    double m_ref_depth;
    GeographicLib::LocalCartesian m_ltp_proj;


};

#endif
