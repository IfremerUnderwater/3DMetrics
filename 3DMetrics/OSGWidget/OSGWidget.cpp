﻿#include "OSGWidget.h"
//#include "PickHandler.h"

#include <QDebug>
#include <QKeyEvent>
#include <QWheelEvent>


#ifdef WITH_OSG

#include <osg/Camera>

#include <osg/DisplaySettings>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/StateSet>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>
#include <osgUtil/Optimizer>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/GUIEventAdapter>

#include <cassert>

#include <stdexcept>
#include <vector>


#include <osg/Referenced>
#include <osg/LineSegment>
#include <osg/Geometry>



OSGWidget::OSGWidget( QWidget* parent,
                      const QGLWidget* shareWidget,
                      Qt::WindowFlags f )
    : QGLWidget( parent,
                 shareWidget,
                 f )
    , m_graphicsWindow( new osgViewer::GraphicsWindowEmbedded( this->x(),
                                                               this->y(),
                                                               this->width(),
                                                               this->height() ) )
    , m_viewer( new osgViewer::CompositeViewer )
    , m_selectionActive( false )
    , m_selectionFinished( true )
    , m_stereoActive(false)

{

    //Initialize widget tool state
    m_tool_state = IDLE_STATE;

    float aspectRatio = static_cast<float>( this->width() ) / static_cast<float>( this->height() );

    osg::Camera* camera = new osg::Camera;
    camera->setViewport( 0, 0, this->width() , this->height() );

    // Set clear color
    QColor clearColor = QColor(0,0,0);
    camera->setClearColor( osg::Vec4( clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF() ) );

    camera->setProjectionMatrixAsPerspective( 30.f, aspectRatio, 1.f, 1000.f );
    camera->setGraphicsContext( m_graphicsWindow );

    osgViewer::View* view = new osgViewer::View;
    view->setCamera( camera );

    view->addEventHandler( new osgViewer::StatsHandler );

    osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
    //manipulator->setAllowThrow( false );

    view->setCameraManipulator( manipulator );

    m_viewer->addView( view );
    m_viewer->setThreadingModel( osgViewer::CompositeViewer::SingleThreaded );
    m_viewer->realize();

    this->setAutoBufferSwap( false );

    // This ensures that the widget will receive keyboard events. This focus
    // policy is not set by default. The default, Qt::NoFocus, will result in
    // keyboard events that are ignored.
    this->setFocusPolicy( Qt::StrongFocus );
    this->setMinimumSize( 100, 100 );

    // Ensures that the widget receives mouse move events even though no
    // mouse button has been pressed. We require this in order to let the
    // graphics window switch viewports properly.
    this->setMouseTracking( true );

    connect( &m_timer, SIGNAL(timeout()), this, SLOT(update()) );
    m_timer.start( 10 );

    // Create group that will contain measurement geode and 3D model
    m_group = new osg::Group;
    m_measurement_geode = new osg::Geode;
    m_group->addChild(m_measurement_geode);

}

OSGWidget::~OSGWidget()
{
}

bool OSGWidget::setSceneFromFile(std::string sceneFile_p)
{
    // load the data
    setlocale(LC_ALL, "C");
    //_loadedModel = osgDB::readRefNodeFile(sceneFile_p, new osgDB::Options("noTriStripPolygons"));
    m_loadedModel = osgDB::readRefNodeFile(sceneFile_p);
    osg::StateSet* stateSet = m_loadedModel->getOrCreateStateSet();
    stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    //m_group = dynamic_cast<osg::Group*>(m_loadedModel.get());
    m_group->addChild(m_loadedModel.get());

    if (!m_group)
    {
        std::cout << "No data loaded" << std::endl;
        return false;
    }

    // optimize the scene graph, remove redundant nodes and state etc.
    osgUtil::Optimizer optimizer;
    optimizer.optimize(m_group.get());

    osgViewer::View *view = m_viewer->getView(0);

    view->setSceneData( m_group );

    return true;

}

bool OSGWidget::setSceneData(osg::ref_ptr<osg::Node> sceneData_p)
{
    if (!sceneData_p)
    {
        std::cout << "No data loaded" << std::endl;
        return false;
    }

    m_loadedModel = sceneData_p;


    osgViewer::View *view = m_viewer->getView(0);

    view->setSceneData( m_loadedModel.get() );


    return true;
}

void OSGWidget::setClearColor(double r_p, double g_p, double b_p, double alpha_p)
{
    std::vector<osg::Camera*> cameras;
    m_viewer->getCameras( cameras );

    for (unsigned int i=0; i<cameras.size(); i++){
        cameras[i]->setClearColor( osg::Vec4( r_p, g_p, b_p, alpha_p ));
    }

}

void OSGWidget::clearSceneData()
{
    osgViewer::View *view = m_viewer->getView(0);
    view->setSceneData( 0 );

    m_loadedModel = NULL;
    m_group = NULL;
    m_measurement_geode = NULL;

    m_line_measurement_tool.resetModelData();
    m_line_measurement_tool.resetLineData();

    m_surface_measurement_tool.resetModelData();
    m_surface_measurement_tool.resetSurfaceData();

    m_interest_point_tool.resetModelData();
    m_interest_point_tool.resetInterestPointData();

    // Allocate group and measurement for next model
    m_group = new osg::Group;
    m_measurement_geode = new osg::Geode;
}



void OSGWidget::removeLastMeasurementOfType(ToolState _meas_type)
{
    switch( _meas_type )
    {
    case IDLE_STATE:

        break;

    case LINE_MEASUREMENT_STATE:
    {
        m_line_measurement_tool.removeLastMeasurement(m_measurement_geode);
    }
        break;

    case SURFACE_MEASUREMENT_STATE:
    {
        m_surface_measurement_tool.removeLastMeasurement(m_measurement_geode);
    }
        break;

    case INTEREST_POINT_STATE:
    {
        m_interest_point_tool.removeLastMeasurement(m_measurement_geode);
    }
        break;

    case CUT_AREA_TOOL_STATE:

        break;

    case ZOOM_IN_TOOL_STATE:

        break;

    case ZOOM_OUT_TOOL_STATE:

        break;

    case  FULL_SCREEN_TOOL_STATE:

        break;

    case CROP_TOOL_STATE:

        break;
    }

    m_group->removeChild(m_measurement_geode);
    m_group->addChild(m_measurement_geode);

}


void OSGWidget::removeMeasurementOfType(ToolState _meas_type, int _meas_index)
{
    switch( _meas_type )
    {
    case IDLE_STATE:

        break;

    case LINE_MEASUREMENT_STATE:
    {
        m_line_measurement_tool.removeMeasurement(m_measurement_geode, _meas_index);
    }
        break;

    case SURFACE_MEASUREMENT_STATE:
    {
        m_surface_measurement_tool.removeMeasurement(m_measurement_geode, _meas_index);
    }
        break;

    case INTEREST_POINT_STATE:
    {
        m_interest_point_tool.removeMeasurement(m_measurement_geode, _meas_index);
    }
        break;

    case CUT_AREA_TOOL_STATE:

        break;

    case ZOOM_IN_TOOL_STATE:

        break;

    case ZOOM_OUT_TOOL_STATE:

        break;

    case  FULL_SCREEN_TOOL_STATE:

        break;

    case CROP_TOOL_STATE:

        break;
    }

    m_group->removeChild(m_measurement_geode);
    m_group->addChild(m_measurement_geode);
}


// hide/show measurement method
void OSGWidget::hideShowMeasurementOfType(ToolState _meas_type, int _meas_index, bool _visible)
{
    switch( _meas_type )
    {
    case IDLE_STATE:

        break;

    case LINE_MEASUREMENT_STATE:
    {
        m_line_measurement_tool.hideShowMeasurement(m_measurement_geode, _meas_index, _visible);
    }
        break;

    case SURFACE_MEASUREMENT_STATE:
    {
        m_surface_measurement_tool.hideShowMeasurement(m_measurement_geode, _meas_index, _visible);
    }
        break;

    case INTEREST_POINT_STATE:
    {
        m_interest_point_tool.hideShowMeasurement(m_measurement_geode, _meas_index, _visible);
    }
        break;

    case CUT_AREA_TOOL_STATE:

        break;

    case ZOOM_IN_TOOL_STATE:

        break;

    case ZOOM_OUT_TOOL_STATE:

        break;

    case  FULL_SCREEN_TOOL_STATE:

        break;

    case CROP_TOOL_STATE:

        break;
    }

    m_group->removeChild(m_measurement_geode);
    m_group->addChild(m_measurement_geode);
}




QMap<int, osg::ref_ptr<osg::Vec3dArray> > OSGWidget::getPointsCoordinates(QString _measur_type)
{
    if(_measur_type == "Distance measurement")
    {
         return m_line_measurement_tool.getMeasurementsHistoryQmap();
    }
    else if(_measur_type == "Surface measurement")
    {
        return m_surface_measurement_tool.getMeasurementsHistoryQmap();
    }
    else if(_measur_type == "Interest Point measurement")
    {
        return m_interest_point_tool.getMeasurementsHistoryQmap();
    }
}



QMap<int,int> OSGWidget::getMeasurPtsNumber(QString _measur_type)
{
    if(_measur_type == "Distance measurement")
    {
         return m_line_measurement_tool.getMeasurPtsNumber();
    }
    else if(_measur_type == "Surface measurement")
    {
        return m_surface_measurement_tool.getMeasurPtsNumber();
    }
    else if(_measur_type == "Interest Point measurement")
    {
        return m_interest_point_tool.getMeasurPtsNumber();
    }
}



QMap<int,int> OSGWidget::getMeasurLinesNumber(QString _measur_type)
{
    if(_measur_type == "Distance measurement")
    {
         return m_line_measurement_tool.getMeasurLinesNumber();
    }
    else if(_measur_type == "Surface measurement")
    {
        return m_surface_measurement_tool.getMeasurLinesNumber();
    }
    else if(_measur_type == "Interest Point measurement")
    {
        return m_interest_point_tool.getMeasurLinesNumber();
    }
}




void OSGWidget::paintEvent( QPaintEvent* /* paintEvent */ )
{
    this->makeCurrent();

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );

    this->paintGL();

#ifdef WITH_SELECTION_PROCESSING
    if( selectionActive_ && !selectionFinished_ )
    {
        painter.setPen( Qt::black );
        painter.setBrush( Qt::transparent );
        painter.drawRect( makeRectangle( selectionStart_, selectionEnd_ ) );
    }
#endif

    painter.end();

    this->swapBuffers();
    this->doneCurrent();
}

void OSGWidget::paintGL()
{
    m_viewer->frame();
}

void OSGWidget::resizeGL( int width, int height )
{
    this->getEventQueue()->windowResize( this->x(), this->y(), width, height );
    m_graphicsWindow->resized( this->x(), this->y(), width, height );

    this->onResize( width, height );
}

void OSGWidget::keyPressEvent( QKeyEvent* event )
{
    QString keyString   = event->text();
    const char* keyData = keyString.toLocal8Bit().data();

    if( event->key() == Qt::Key_3 )
    {
        /*#ifdef WITH_SELECTION_PROCESSING
        selectionActive_ = !selectionActive_;
#endif*/

        // Further processing is required for the statistics handler here, so we do
        // not return right away.
        if (m_stereoActive){
            osg::DisplaySettings::instance()->setStereo(false);
            m_stereoActive=false;
        }
        else{
            osg::DisplaySettings::instance()->setStereo(true);
            m_stereoActive=true;
        }
    }
    else if( event->key() == Qt::Key_D )
    {
        osgDB::writeNodeFile( *m_viewer->getView(0)->getSceneData(),
                              "/tmp/sceneGraph.osg" );

        return;
    }
    else if( event->key() == Qt::Key_H )
    {
        this->onHome();
        return;
    }

    this->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KeySymbol( *keyData ) );
}

void OSGWidget::keyReleaseEvent( QKeyEvent* event )
{
    QString keyString   = event->text();
    const char* keyData = keyString.toLocal8Bit().data();

    this->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KeySymbol( *keyData ) );
}

void OSGWidget::mouseMoveEvent( QMouseEvent* event )
{
    // Note that we have to check the buttons mask in order to see whether the
    // left button has been pressed. A call to `button()` will only result in
    // `Qt::NoButton` for mouse move events.
    if( m_selectionActive && event->buttons() & Qt::LeftButton )
    {
        m_selectionEnd = event->pos();

        // Ensures that new paint events are created while the user moves the
        // mouse.
        this->update();
    }
    else
    {
        this->getEventQueue()->mouseMotion( static_cast<float>( event->x() ),
                                            static_cast<float>( event->y() ) );
    }
}


void OSGWidget::mousePressEvent( QMouseEvent* event )
{
    // Selection processing
    if( m_selectionActive && event->button() == Qt::LeftButton )
    {
        m_selectionStart    = event->pos();
        m_selectionEnd      = m_selectionStart; // Deletes the old selection
        m_selectionFinished = false;           // As long as this is set, the rectangle will be drawn
    }

    // Normal processing
    else
    {
        // 1 = left mouse button
        // 2 = middle mouse button
        // 3 = right mouse button

        unsigned int button = 0;

        switch( event->button() )
        {
        case Qt::LeftButton:
        {
            button = 1;


            switch( m_tool_state )
            {
            case IDLE_STATE:

                break;

            case LINE_MEASUREMENT_STATE:
            {

                bool inter_exists;
                osg::Vec3d inter_point;
                getIntersectionPoint(event->x(), event->y(), inter_point, inter_exists);
                if(inter_exists){

                    m_line_measurement_tool.pushNewPoint(m_measurement_geode,inter_point);
                    m_group->removeChild(m_measurement_geode);
                    m_group->addChild(m_measurement_geode);

                }

            }
                break;

            case SURFACE_MEASUREMENT_STATE:
            {

                bool inter_exists;
                osg::Vec3d inter_point;
                getIntersectionPoint(event->x(), event->y(), inter_point, inter_exists);
                if(inter_exists){

                    m_surface_measurement_tool.pushNewPoint(m_measurement_geode,inter_point);
                    m_group->removeChild(m_measurement_geode);
                    m_group->addChild(m_measurement_geode);

                }

            }
                break;

            case INTEREST_POINT_STATE:
            {

                bool inter_exists;
                osg::Vec3d inter_point;
                getIntersectionPoint(event->x(), event->y(), inter_point, inter_exists);
                if(inter_exists){

                    m_interest_point_tool.pushNewPoint(m_measurement_geode,inter_point);
                    m_group->removeChild(m_measurement_geode);
                    m_group->addChild(m_measurement_geode);

                }

                emit si_showInterestPointMeasurementSavingPopup(m_interest_point_tool.interestPointCoordinates(),
                                                                m_interest_point_tool.getTypeOfMeasur(),
                                                                m_interest_point_tool.getMeasurementCounter());
                m_interest_point_tool.endMeasurement();
                emit si_endMeasur();
                slot_setInIdleState();

            }

                break;

            case CUT_AREA_TOOL_STATE:

                break;

            case ZOOM_IN_TOOL_STATE:

                break;

            case ZOOM_OUT_TOOL_STATE:

                break;

            case  FULL_SCREEN_TOOL_STATE:

                break;

            case CROP_TOOL_STATE:

                break;
            }
        }
            break;

        case Qt::MiddleButton:
        {
            button = 2;

            switch( m_tool_state )
            {
            case IDLE_STATE:

                break;

            case LINE_MEASUREMENT_STATE:
            {
                if(m_line_measurement_tool.getNumberOfPoints() >= 3)
                {
                    m_line_measurement_tool.closeLoop(m_measurement_geode);
                    emit sig_showMeasurementSavingPopup(m_line_measurement_tool.closedLineLength(),
                                                        m_line_measurement_tool.getTypeOfMeasur(),
                                                        m_line_measurement_tool.getMeasurementCounter());
                    m_line_measurement_tool.endMeasurement();
                    emit si_endMeasur();
                    slot_setInIdleState();
                }
            }
                break;

            case SURFACE_MEASUREMENT_STATE:

                break;

            case INTEREST_POINT_STATE:

                break;

            case CUT_AREA_TOOL_STATE:

                break;

            case ZOOM_IN_TOOL_STATE:

                break;

            case ZOOM_OUT_TOOL_STATE:

                break;

            case  FULL_SCREEN_TOOL_STATE:

                break;

            case CROP_TOOL_STATE:

                break;
            }
        }
            break;

        case Qt::RightButton:
        {
            button = 3;

            switch( m_tool_state )
            {
            case IDLE_STATE:

                break;

            case LINE_MEASUREMENT_STATE:
            {
                if(m_line_measurement_tool.getNumberOfPoints() >= 2)
                {
                    emit sig_showMeasurementSavingPopup(m_line_measurement_tool.lineLength(),
                                                        m_line_measurement_tool.getTypeOfMeasur(),
                                                        m_line_measurement_tool.getMeasurementCounter());
                    m_line_measurement_tool.endMeasurement();
                    emit si_endMeasur();
                    slot_setInIdleState();
                }
            }
                break;

            case SURFACE_MEASUREMENT_STATE:
            {
                if(m_surface_measurement_tool.getNumberOfPoints() >= 3)
                {
                    m_surface_measurement_tool.closeLoop(m_measurement_geode);
                    emit sig_showMeasurementSavingPopup(m_surface_measurement_tool.getArea(),
                                                        m_surface_measurement_tool.getTypeOfMeasur(),
                                                        m_surface_measurement_tool.getMeasurementCounter());
                    m_surface_measurement_tool.endMeasurement();
                    emit si_endMeasur();
                    slot_setInIdleState();
                }
            }

                break;

            case INTEREST_POINT_STATE:

                break;

            case CUT_AREA_TOOL_STATE:

                break;

            case ZOOM_IN_TOOL_STATE:

                break;

            case ZOOM_OUT_TOOL_STATE:

                break;

            case  FULL_SCREEN_TOOL_STATE:

                break;

            case CROP_TOOL_STATE:

                break;
            }
        }
            break;

        default:
            break;
        }


        this->getEventQueue()->mouseButtonPress( static_cast<float>( event->x() ),
                                                 static_cast<float>( event->y() ),
                                                 button );

    }


}



void OSGWidget::getIntersectionPoint(int _x, int _y, osg::Vec3d &_inter_point, bool &_inter_exists)
{

    osgUtil::LineSegmentIntersector::Intersections intersections;

    osgViewer::View *view = m_viewer->getView(0);

    // if we click on the object
    if (view->computeIntersections(_x, this->size().height()-_y,intersections))
    {
        _inter_exists = true;

        osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();

        if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
        {
            // the geodes are identified by name.
            std::cout<<"Object \""<<hitr->nodePath.back()->getName()<<"\""<<std::endl;
        }
        else if (hitr->drawable.valid())
        {
            std::cout<<"Object \""<<hitr->drawable->className()<<"\""<<std::endl;
        }


        // we get the intersections in a osg::Vec3d
        _inter_point = hitr->getLocalIntersectPoint();

    }else{
        _inter_exists = false;
    }
}

void OSGWidget::mouseReleaseEvent(QMouseEvent* event)
{
    // Selection processing: Store end position and obtain selected objects
    // through polytope intersection.
    if( m_selectionActive && event->button() == Qt::LeftButton )
    {
        m_selectionEnd      = event->pos();
        m_selectionFinished = true; // Will force the painter to stop drawing the
        // selection rectangle

    }

    // Normal processing
    else
    {
        // 1 = left mouse button
        // 2 = middle mouse button
        // 3 = right mouse button

        unsigned int button = 0;

        switch( event->button() )
        {
        case Qt::LeftButton:
            button = 1;
            break;

        case Qt::MiddleButton:
            button = 2;
            break;

        case Qt::RightButton:
            button = 3;
            break;

        default:
            break;
        }

        this->getEventQueue()->mouseButtonRelease( static_cast<float>( event->x() ),
                                                   static_cast<float>( event->y() ),
                                                   button );



    }
}

void OSGWidget::wheelEvent( QWheelEvent* event )
{
    // Ignore wheel events as long as the selection is active.
    if( m_selectionActive )
        return;

    event->accept();
    int delta = event->delta();

    osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?   osgGA::GUIEventAdapter::SCROLL_UP
                                                                 : osgGA::GUIEventAdapter::SCROLL_DOWN;

    this->getEventQueue()->mouseScroll( motion );
}

bool OSGWidget::event( QEvent* event )
{
    bool handled = QGLWidget::event( event );

    // This ensures that the OSG widget is always going to be repainted after the
    // user performed some interaction. Doing this in the event handler ensures
    // that we don't forget about some event and prevents duplicate code.
    switch( event->type() )
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::Wheel:
        this->update();
        break;

    default:
        break;
    }

    return handled;
}



void OSGWidget::onHome()
{
    osgViewer::ViewerBase::Views views;
    m_viewer->getViews( views );

    for( std::size_t i = 0; i < views.size(); i++ )
    {
        osgViewer::View* view = views.at(i);
        view->home();
    }
}

void OSGWidget::onResize( int width, int height )
{
    std::vector<osg::Camera*> cameras;
    m_viewer->getCameras( cameras );

    cameras[0]->setViewport( 0, 0, width, height );
    //cameras[1]->setViewport( this->width() / 2, 0, this->width() / 2, this->height() );
}

osgGA::EventQueue* OSGWidget::getEventQueue() const
{
    osgGA::EventQueue* eventQueue = m_graphicsWindow->getEventQueue();

    if( eventQueue )
        return eventQueue;
    else
        throw std::runtime_error( "Unable to obtain valid event queue");
}



/******************SLOTS FOR STATES***********************************************/

void OSGWidget::slot_setInIdleState()
{
    m_tool_state = IDLE_STATE;
    qDebug() << "OSGWidget is in state : " << m_tool_state;
}

void OSGWidget::slot_setInLineMeasurementState()
{
    m_tool_state = LINE_MEASUREMENT_STATE;
    qDebug() << "OSGWidget is in state : " << m_tool_state;
}


void OSGWidget::slot_setInSurfaceMeasurementState()
{
    m_tool_state = SURFACE_MEASUREMENT_STATE;
    qDebug() << "OSGWidget is in state : " << m_tool_state;
}


void OSGWidget::slot_setInInterestPointState()
{
    m_tool_state = INTEREST_POINT_STATE;
    qDebug() << "OSGWidget is in state : " << m_tool_state;


}


void OSGWidget::slot_setInCutAreaState()
{
    m_tool_state = CUT_AREA_TOOL_STATE;
    qDebug() << "OSGWidget is in state : " << m_tool_state;
}


void OSGWidget::slot_setInZoomInState()
{
    m_tool_state = ZOOM_IN_TOOL_STATE;
    qDebug() << "OSGWidget is in state : " << m_tool_state;
}


void OSGWidget::slot_setInZoomOutState()
{
    m_tool_state = ZOOM_OUT_TOOL_STATE;
    qDebug() << "OSGWidget is in state : " << m_tool_state;
}


void OSGWidget::slot_setInFullScreenState()
{
    m_tool_state = FULL_SCREEN_TOOL_STATE;
    qDebug() << "OSGWidget is in state : " << m_tool_state;
}


void OSGWidget::slot_setInCropState()
{
    m_tool_state = CROP_TOOL_STATE;
    qDebug() << "OSGWidget is in state : " << m_tool_state;
}



void OSGWidget::sl_resetMeasur()
{
    if(m_tool_state == LINE_MEASUREMENT_STATE)
    {
        m_line_measurement_tool.endMeasurement();
        m_line_measurement_tool.removeLastMeasurement(m_measurement_geode);
    }

    else if(m_tool_state == SURFACE_MEASUREMENT_STATE)
    {
        m_surface_measurement_tool.endMeasurement();
        m_surface_measurement_tool.removeLastMeasurement(m_measurement_geode);
    }

    else if(m_tool_state == INTEREST_POINT_STATE)
    {
        m_interest_point_tool.endMeasurement();
        m_interest_point_tool.removeLastMeasurement(m_measurement_geode);
    }

    emit si_returnIdleState();

}



#else
OSGWidget::OSGWidget( QWidget* parent,
                      const QGLWidget* shareWidget,
                      Qt::WindowFlags f){
    Q_UNUSED(parent)
    Q_UNUSED(shareWidget)
    Q_UNUSED(f)
    qCritical("You cannot use OSG as your version is compiled without WITH_OSG\n");
}

OSGWidget::~OSGWidget(){}

#endif
