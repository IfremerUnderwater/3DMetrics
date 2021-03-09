#include "osg_widget.h"

#include <QDebug>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QFileInfo>
#include <QString>
#include <QDir>

#include <osg/Camera>
#include <osg/MatrixTransform>
#include <osg/DisplaySettings>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/LOD>
//#include <osg/PagedLOD>

#include <osg/TexGen>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>
#include <osgUtil/Optimizer>
#include <osgUtil/Simplifier>
// too slow
//#include <osgUtil/DelaunayTriangulator>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/config/SingleWindow>
#include <osgGA/GUIEventAdapter>

#include <cassert>

#include <stdexcept>
#include <vector>


#include <osg/Referenced>
#include <osg/LineSegment>
#include <osg/Geometry>
#include <osg/Point>
#include <osg/LineWidth>

#include <osg/PolygonMode>
#include <QProcess>
#include <math.h>
#include <limits>

#include <osg/AlphaFunc>
#include <osg/BlendFunc>

#include "box_visitor.h"
#include "clip_model_visitor.h"

#include "minmax_computation_visitor.h"
#include "geometry_type_count_visitor.h"
#include "shader_color.h"
#include "shader_builder.h"

#include "smartlod.h"
#include "grid_file_processor.h"
#include "lod_tools.h"

#include "snap_geotiff_image.h"
#include "elevation_map_creator.h"

class KeyboardEventHandler : public osgGA::GUIEventHandler
{
    OSGWidget *m_osgWidget;
public:

    KeyboardEventHandler(osg::StateSet* stateset, OSGWidget *_osgWidget):
        _stateset(stateset), m_osgWidget(_osgWidget)
    {
        _point = new osg::Point;
        _point->setDistanceAttenuation(osg::Vec3(0.0,0.0000,0.05f));
        _point->setSize(30);
        _stateset->setAttribute(_point.get());

        _line_width = new osg::LineWidth();
        _line_width->setWidth(4.0);
        _stateset->setAttribute(_line_width.get());

    }

    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
    {
        switch(ea.getEventType())
        {
        case(osgGA::GUIEventAdapter::KEYDOWN):
        {
            if (ea.getKey()=='+' || ea.getKey()==osgGA::GUIEventAdapter::KEY_KP_Add)
            {
                changePointSize(1.0f);
                changeLineWidth(1.0f);
                return true;
            }
            else if (ea.getKey()=='-' || ea.getKey()==osgGA::GUIEventAdapter::KEY_KP_Subtract)
            {
                changePointSize(-1.0f);
                changeLineWidth(-1.0f);
                return true;
            }
            else if (ea.getKey()=='<')
            {
                changePointAttenuation(1.1f);
                return true;
            }
            else if (ea.getKey()=='>')
            {
                changePointAttenuation(1.0f/1.1f);
                return true;
            }
            else if (ea.getKey()==osgGA::GUIEventAdapter::KEY_L)
            {
                // handle light on shaders
                m_osgWidget->enableLight(_stateset->getMode(GL_LIGHTING) == osg::StateAttribute::OFF);
                //                if (_stateset->getMode(GL_LIGHTING) == osg::StateAttribute::OFF)
                //                    _stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
                //                else
                //                    _stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
                return true;
            }
            break;
        }
        default:
            break;
        }
        return false;
    }

    float getPointSize() const
    {
        return _point->getSize();
    }

    float getLineWidth() const
    {
        return _line_width->getWidth();
    }

    void setPointSize(float psize)
    {
        if (psize>0.0)
        {
            _point->setSize(psize);
        }
        std::cout<<"Point size "<<psize<<std::endl;
    }

    void setLineWidth(float pwidth)
    {
        if (pwidth>0.0)
        {
            _line_width->setWidth(pwidth);
        }
        std::cout<<"Line width "<<pwidth<<std::endl;
    }

    void changePointSize(float delta)
    {
        setPointSize(getPointSize()+delta);
    }

    void changeLineWidth(float delta)
    {
        setLineWidth(getLineWidth()+delta);
    }

    void changePointAttenuation(float scale)
    {
        _point->setDistanceAttenuation(_point->getDistanceAttenuation()*scale);
    }

    osg::ref_ptr<osg::StateSet> _stateset;
    osg::ref_ptr<osg::Point>    _point;
    osg::ref_ptr<osg::LineWidth> _line_width;

};


const char *const OSGWidget::MEASUREMENT_NAME = "3DMeasurement";

OSGWidget::OSGWidget(QWidget* parent)
    : QOpenGLWidget( parent)
    , m_graphicsWindow( new osgViewer::GraphicsWindowEmbedded( this->x(),
                                                               this->y(),
                                                               this->width(),
                                                               this->height() ) )
    , m_viewer( new osgViewer::CompositeViewer )
    , m_ctrl_pressed(false)
    , m_fake_middle_click_activated(false)
    , m_zScale(1.0)
{

    m_ref_lat_lon.setX(INVALID_VALUE);
    m_ref_lat_lon.setY(INVALID_VALUE);
    m_ref_alt = INVALID_VALUE;

#ifdef __APPLE__
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("Plugins");
    osgDB::Registry::instance()->setLibraryFilePathList(dir.absolutePath().toStdString());
#endif

    float aspectRatio = static_cast<float>( this->width() ) / static_cast<float>( this->height() );

    osg::Camera* camera = new osg::Camera;
    camera->setViewport( 0, 0, this->width() , this->height() );

    // tweak unique point not drawing
    osg::CullStack::CullingMode cullingMode = camera->getCullingMode();
    cullingMode &= ~(osg::CullStack::SMALL_FEATURE_CULLING);
    camera->setCullingMode(cullingMode);

    // Set clear color
    QColor clearColor = QColor(0,0,0);
    camera->setClearColor( osg::Vec4( clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF() ) );

    camera->setGraphicsContext( m_graphicsWindow );
    camera->setProjectionMatrixAsPerspective( 30.0f, aspectRatio, 1.f, 1000.f );

    osgViewer::View* view = new osgViewer::View;
    view->setCamera( camera );

    view->addEventHandler( new osgViewer::StatsHandler );
    view->addEventHandler(new KeyboardEventHandler(view->getCamera()->getOrCreateStateSet(), this));


    osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
    manipulator->setAllowThrow( false );

    view->setCameraManipulator( manipulator );

    m_viewer->addView( view );
    m_viewer->setThreadingModel( osgViewer::CompositeViewer::SingleThreaded );
    // others threading modes cause SEGV
    m_viewer->realize();

    // This ensures that the widget will receive keyboard events. This focus
    // policy is not set by default. The default, Qt::NoFocus, will m_image in
    // keyboard events that are ignored.
    this->setFocusPolicy( Qt::StrongFocus );
    this->setMinimumSize( 100, 100 );

    // Ensures that the widget receives mouse move events even though no
    // mouse button has been pressed. We require this in order to let the
    // graphics window switch viewports properly.
    this->setMouseTracking( true );

    //    connect( &m_timer, SIGNAL(timeout()), this, SLOT(update()) );
    //    m_timer.start( 40 ); //10 );
    // TODO : call update() all modification or visibility changed
    m_viewer->setRunFrameScheme( osgViewer::ViewerBase::ON_DEMAND );

    // Create group that will contain measurement geode and 3D model
    m_globalGroup = new osg::Group;

    m_modelsGroup = new osg::Group;
    m_geodesGroup = new osg::Group;

    m_globalGroup->addChild(m_modelsGroup);
    m_globalGroup->addChild(m_geodesGroup);

    // for Z scale management
    m_matrixTransform = new osg::MatrixTransform;
    m_matrixTransform->setMatrix(osg::Matrix::scale(1.0, 1.0, m_zScale));
    m_matrixTransform->addChild(m_globalGroup);

    // use models' min max as default
    m_useDisplayZMinMax = false;

    // don't show zscale by default
    m_showZScale = false;

    m_modelsZMin = 0;
    m_modelsZMax= 0;

    m_displayZMin = 0;
    m_displayZMax = 0;

    m_colorPalette = ShaderColor::Rainbow;
    m_overlay = new OverlayWidget(this);
    m_overlay->setColorPalette(m_colorPalette);
    m_overlay->setMinMax(m_displayZMin, m_displayZMax);
    if(m_showZScale)
        m_overlay->show();
    else
        m_overlay->hide();
}

OSGWidget::~OSGWidget()
{
}

bool OSGWidget::setSceneFromFile(std::string _scene_file)
{
    osg::ref_ptr<osg::Node> node = createNodeFromFile(_scene_file);
    if(!node)
        return false;

    return addNodeToScene(node);
}

///
/// \brief createNodeFromFile load a scene from a 3D file
/// \param _sceneFile path to any 3D file supported by osg
/// \return node if loading succeded
///
osg::ref_ptr<osg::Node> OSGWidget::createNodeFromFile(std::string _scene_file)
{
    osg::ref_ptr<osg::MatrixTransform> model_transform;
    // load the data
    // in main.cpp
    // setlocale(LC_ALL, "C");

    QFileInfo scene_info(QString::fromStdString(_scene_file));
    std::string scene_file;

    QPointF local_lat_lon;
    double local_alt;

    if (scene_info.suffix()==QString("kml")){
        m_kml_handler.readFile(_scene_file);
        scene_file = scene_info.absoluteDir().filePath(QString::fromStdString(m_kml_handler.getModelPath())).toStdString();
        local_lat_lon.setX(m_kml_handler.getModelLat());
        local_lat_lon.setY(m_kml_handler.getModelLon());
        local_alt = m_kml_handler.getModelAlt();
    }else{
        scene_file = _scene_file;
        local_lat_lon.setX(0);
        local_lat_lon.setY(0);
        local_alt = 0;
    }

    osg::ref_ptr<osg::Node> model_node=osgDB::readRefNodeFile(scene_file, new osgDB::Options("noRotation"));

    if (!model_node)
    {
        std::cout << "No data loaded" << std::endl;
        return model_transform;

    }

    //    // TEST
    //    osg::ref_ptr<osg::Group> group;
    //    GridFileProcessor gfp;

    //    //bool status =gfp.createLODTilesFromNodeGlobalSimplify(model_node, scene_file, 3, 4, true, 40.0f);
    //    //group = gfp.loadTiles(scene_file);
    //    group = gfp.loadSmartLODTiles(scene_file);
    //    // hack
    //    LODTools::applyLODValuesInTree(group, 40.0, 200.0);
    //    model_node = group;

    // Transform model
    model_transform = new osg::MatrixTransform;
    if (m_ref_alt == INVALID_VALUE){
        m_ref_lat_lon = local_lat_lon;
        m_ref_alt = local_alt;
        m_ltp_proj.Reset(m_ref_lat_lon.x(), m_ref_lat_lon.y(),m_ref_alt);

        osg::Matrix matrix = osg::Matrix::identity();
        model_transform->setMatrix(matrix);
    }else{
        double N,E,U;
        m_ltp_proj.Forward(local_lat_lon.x(), local_lat_lon.y(), local_alt, E, N, U);

        model_transform->setMatrix(osg::Matrix::translate(E,N,U));
    }

    model_transform->addChild(model_node);

    return model_transform;
}

///
/// \brief createNodeFromFile load a scene from a 3D file
/// \param _sceneFile path to any 3D file supported by osg
/// \param _loading_mode loading mode used
/// \param _subdir tiles' subdirectory
/// \param _nTilesX # tiles tocreate in X
/// \param _nTilesY # tiles to create in Y
/// \return node if loading succeded
///
osg::ref_ptr<osg::Node> OSGWidget::createNodeFromFile(std::string _scene_file, LoadingMode _loading_mode, std::string _subdir, int _nTilesX, int _nTilesY)
{
    osg::ref_ptr<osg::MatrixTransform> model_transform;
    // load the data
    // in main.cpp
    // setlocale(LC_ALL, "C");

    QFileInfo scene_info(QString::fromStdString(_scene_file));
    std::string scene_file;

    QPointF local_lat_lon;
    double local_alt;

    if (scene_info.suffix()==QString("kml")){
        m_kml_handler.readFile(_scene_file);
        scene_file = scene_info.absoluteDir().filePath(QString::fromStdString(m_kml_handler.getModelPath())).toStdString();
        local_lat_lon.setX(m_kml_handler.getModelLat());
        local_lat_lon.setY(m_kml_handler.getModelLon());
        local_alt = m_kml_handler.getModelAlt();
    }else{
        scene_file = _scene_file;
        local_lat_lon.setX(0);
        local_lat_lon.setY(0);
        local_alt = 0;
    }

    osg::ref_ptr<osg::Node> model_node;

    osg::ref_ptr<osg::Group> group;
    GridFileProcessor gfp;
    bool status;

    switch(_loading_mode)
    {
    case LoadingModeDefault:
        model_node=osgDB::readRefNodeFile(scene_file, new osgDB::Options("noRotation"));
        break;

        // Tiles
    case LoadingModeLODTiles:
        group = gfp.loadTiles(scene_file);
        model_node = group;
        break;

    case LoadingModeLODTilesDir:
        group = gfp.loadTiles(scene_file, _subdir);
        model_node = group;
        break;

    case LoadingModeSmartLODTiles:
        group = gfp.loadSmartLODTiles(scene_file);
        model_node = group;
        break;

    case LoadingModeSmartLODTilesDir:
        group = gfp.loadSmartLODTiles(scene_file, _subdir);
        model_node = group;
        break;

    case LoadingModeUseOSGB:
    {
        QFileInfo f((scene_file + ".osgb").c_str());
        if(f.exists())
        {
            model_node=osgDB::readRefNodeFile(scene_file + ".osgb", new osgDB::Options("noRotation"));
        }
        else
        {
            model_node=osgDB::readRefNodeFile(scene_file, new osgDB::Options("noRotation"));
        }
    }
        break;

        // Build tiles (could be slow)
        // (in current directory)
    case LoadingModeBuildLODTiles:
        status =gfp.createLODTilesFromNodeGlobalSimplify(model_node, scene_file, _nTilesX, _nTilesY, true, 40.0f, 200.0f);
        //group = gfp.loadTiles(scene_file);
        group = gfp.loadSmartLODTiles(scene_file);
        // hack
        LODTools::applyLODValuesInTree(group, 40.0, 200.0);
        model_node = group;
        break;

    case LoadingModeBuildOSGB:
        model_node=osgDB::readRefNodeFile(scene_file, new osgDB::Options("noRotation"));
        osgDB::writeNodeFile(*model_node,
                             scene_file + ".osgb",
                             new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib"));
        break;

        // use createLODNodeFromFiles instead
        //    case LoadingModeBuildOSGBLOD:
        //        break;

    default:
        // do not load
        break;
    }


    if (!model_node)
    {
        std::cout << "No data loaded" << std::endl;
        return model_transform;

    }

    //    // TEST
    //    osg::ref_ptr<osg::Group> group;
    //    GridFileProcessor gfp;

    //    //bool status =gfp.createLODTilesFromNodeGlobalSimplify(model_node, scene_file, 3, 4, true, 40.0f);
    //    //group = gfp.loadTiles(scene_file);
    //    group = gfp.loadSmartLODTiles(scene_file);
    //    // hack
    //    LODTools::applyLODValuesInTree(group, 40.0, 200.0);
    //    model_node = group;

    // Transform model
    model_transform = new osg::MatrixTransform;
    if (m_ref_alt == INVALID_VALUE){
        m_ref_lat_lon = local_lat_lon;
        m_ref_alt = local_alt;
        m_ltp_proj.Reset(m_ref_lat_lon.x(), m_ref_lat_lon.y(),m_ref_alt);

        osg::Matrix matrix = osg::Matrix::identity();
        model_transform->setMatrix(matrix);
    }else{
        double N,E,U;
        m_ltp_proj.Forward(local_lat_lon.x(), local_lat_lon.y(), local_alt, E, N, U);

        model_transform->setMatrix(osg::Matrix::translate(E,N,U));
    }

    model_transform->addChild(model_node);

    return model_transform;
}


///
/// \brief createNodeFromFile load a scene from a 3D file
/// \param _sceneFile path to any 3D file supported by osg
/// \return node if loading succeded
///
osg::ref_ptr<osg::Node> OSGWidget::createNodeFromFileWithGDAL(std::string _scene_file, LoadingMode _mode, std::string _tileDir)
{
    osg::ref_ptr<osg::MatrixTransform> model_transform;

    QPointF local_lat_lon;
    double local_alt;

    GridFileProcessor processor;
    osg::ref_ptr<osg::Group> group;

    // get subdirectory
    std::string subdir = _tileDir;
    switch(_mode)
    {
    case LoadingModePoint:
    case LoadingModeTriangle:
    case LoadingModeTriangleNormals:
    case LoadingModeTrianglePoint:
        group = processor.loadGridFile(_scene_file, _mode, local_lat_lon, local_alt);
        break;

    case LoadingModeLODTiles:
        processor.getGridLatLonAlt(_scene_file, local_lat_lon, local_alt);
        group = processor.loadTiles(_scene_file, "");
        break;

    case LoadingModeLODTilesDir:
        processor.getGridLatLonAlt(_scene_file, local_lat_lon, local_alt);
        group = processor.loadTiles(_scene_file, subdir);
        break;

    case LoadingModeSmartLODTiles:
        processor.getGridLatLonAlt(_scene_file, local_lat_lon, local_alt);
        group = processor.loadSmartLODTiles(_scene_file, "", 800.0f, 2500.0f);
        break;

    case LoadingModeSmartLODTilesDir:
        processor.getGridLatLonAlt(_scene_file, local_lat_lon, local_alt);
        group = processor.loadSmartLODTiles(_scene_file, subdir, 800.0f, 2500.0f);
        break;

    case LoadingModeBuildLODTiles:
        group = processor.loadGridFileAndBuildTiles(_scene_file, local_lat_lon, local_alt, true);
        break;
    }

    if(group == nullptr)
    {
        std::cout << "GDAL error ; No data loaded" << std::endl;
        return model_transform;
    }

    model_transform = new osg::MatrixTransform;
    if (m_ref_alt == INVALID_VALUE)
    {
        m_ref_lat_lon = local_lat_lon;
        m_ref_alt = local_alt;
        m_ltp_proj.Reset(m_ref_lat_lon.x(), m_ref_lat_lon.y(),m_ref_alt);

        model_transform->setMatrix(osg::Matrix::identity()); //translate(0,0,0));
    }else{
        double N,E,U;
        m_ltp_proj.Forward(local_lat_lon.x(), local_lat_lon.y(), local_alt, E, N, U);

        model_transform->setMatrix(osg::Matrix::translate(E,N,U));
    }

    // TEST save osgb file
    // warning : SmartLOD saving not supported
    if(_mode == LoadingModeTriangle // || _mode == LoadingModeTriangleNormals (not working)
            || _mode == LoadingModeLODTiles || _mode == LoadingModeLODTilesDir)
    {
        std::string path = _scene_file;
        path = path + ".osgb";
        osgDB::writeNodeFile(*group,
                             path,
                             new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib"));

    }
    model_transform->addChild(group);


    return  model_transform;


}

///
/// \brief addNodeToScene add a binary OSG node to the scene
/// \param _node node to be added
/// \param _transparency transparency (default to 0
/// \return true if loading succeded
///
bool OSGWidget::addNodeToScene(osg::ref_ptr<osg::Node> _node, double _transparency) //, bool _buildLOD, std::string _pathToLodFile)
{
    osg::ref_ptr<osg::MatrixTransform> matrix = dynamic_cast<osg::MatrixTransform*>(_node.get());
    osg::ref_ptr<osg::Node> root = matrix->getChild(0);

    // Add model
    m_models.push_back(matrix);
    osg::StateSet* state_set = root->getOrCreateStateSet();
    state_set->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );
    //    state_set->setMode( GL_BLEND, osg::StateAttribute::ON);

    //    // Add the possibility of modifying the transparence
    //    osg::ref_ptr<osg::Material> material = new osg::Material;
    //    // Put the 3D model totally opaque
    //    material->setAlpha( osg::Material::FRONT, 1.0 );
    //    state_set->setAttributeAndModes ( material, osg::StateAttribute::ON );

    //    osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::ONE_MINUS_SRC_ALPHA,osg::BlendFunc::SRC_ALPHA );
    //    state_set->setAttributeAndModes(bf);

    m_modelsGroup->insertChild(0, matrix.get()); // put at the beginning to be drawn first

    // optimize the scene graph, remove redundant nodes and state etc.
    osgUtil::Optimizer optimizer;
    optimizer.optimize(matrix.get(), osgUtil::Optimizer::ALL_OPTIMIZATIONS  | osgUtil::Optimizer::TESSELLATE_GEOMETRY);

    // compute z min/max of 3D model
    MinMaxComputationVisitor minmax;
    matrix->accept(minmax);
    float zmin = minmax.getMin();
    float zmax = minmax.getMax();

    GeometryTypeCountVisitor geomcount;
    matrix->accept(geomcount);

    // save original translation
    //osg::ref_ptr<osg::MatrixTransform> model_transform =  dynamic_cast<osg::MatrixTransform*>(root.get());

    osg::ref_ptr<NodeUserData> data = new NodeUserData();
    data->useShader = false;
    data->zmin = zmin;
    data->zmax = zmax;
    data->zoffset = 0; // will be changed on z offset changed
    data->originalZoffset = matrix->getMatrix().getTrans().z();
    data->hasMesh = geomcount.getNbTriangles() > 0;
    matrix->setUserData(data);

    //configureShaders( root->getOrCreateStateSet() );
    matrix->getOrCreateStateSet()->addUniform( new osg::Uniform( "zmin", zmin));
    matrix->getOrCreateStateSet()->addUniform( new osg::Uniform( "deltaz", zmax - zmin));
    matrix->getOrCreateStateSet()->addUniform( new osg::Uniform( "hasmesh", data->hasMesh));

    setCameraOnNode(matrix);

    home();

    // set transparency
    setNodeTransparency(matrix, _transparency);

    update();

    return true;
}

///
/// \brief createLODFiles
/// \param _node node to process
/// \param _scene_file_basename
/// \param _buildCompoundLOD
/// \return true if succeded
///
bool OSGWidget::createLODFiles(osg::ref_ptr<osg::Node> _node, std::string _scene_file_basename, bool _buildCompoundLOD)
{
    osg::ref_ptr<osg::MatrixTransform> matrix = dynamic_cast<osg::MatrixTransform*>(_node.get());
    osg::ref_ptr<osg::Node> root = matrix->getChild(0);


    std::string name = _scene_file_basename;

    // LOD processing
    std::string path0 = name;
    path0 = path0 + SmartLOD::EXTLOD0; // "-0.osgb";
    osgDB::writeNodeFile(*root,
                         path0,
                         new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib"));


    osgUtil::Simplifier simplifer;

    simplifer.setSampleRatio(0.1f);
    osg::ref_ptr<osg::Node> modelL1 = dynamic_cast<osg::Node *>(root->clone(osg::CopyOp::DEEP_COPY_ALL));
    modelL1->accept(simplifer);
    std::string path1 = name;
    path1 = path1 + SmartLOD::EXTLOD1; //"-1.osgb";
    osgDB::writeNodeFile(*modelL1,
                         path1,
                         new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib"));

    simplifer.setSampleRatio(0.2f);
    osg::ref_ptr<osg::Node> modelL2 = dynamic_cast<osg::Node *>(modelL1->clone(osg::CopyOp::DEEP_COPY_ALL));
    modelL2->accept(simplifer);
    std::string path2 = name;
    path2 = path2 + SmartLOD::EXTLOD2; //"-2.osgb";
    osgDB::writeNodeFile(*modelL2,
                         path2,
                         new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib"));

    // coumpound LOD
    if(_buildCompoundLOD)
    {
        osg::ref_ptr<osg::LOD> lod = new osg::LOD;
        lod->addChild(root, 0,40.0f);
        lod->addChild(modelL1.get(), 40.0f, 200.0f);
        lod->addChild(modelL2, 200.0f, FLT_MAX);
        std::string path = name;
        path = path + ".osgb";
        osgDB::writeNodeFile(*lod,
                             path,
                             new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib"));
    }

    return true;
}

///
/// \brief createLODNodeFromFiles load a scene from a 3D file
/// \param _scene_file_basename path base file name (without "-0.osgb" "-1.osgb" "-2.osgb")
/// \return SmartLOD node if loading succeded
///
osg::ref_ptr<osg::Node>  OSGWidget::createLODNodeFromFiles(std::string _scene_file_basename)
{
    osg::ref_ptr<osg::MatrixTransform> model_transform;

    // load the data
    // in main.cpp
    //setlocale(LC_ALL, "C");
    QFileInfo scene_info(QString::fromStdString(_scene_file_basename));
    std::string scene_file;

    QPointF local_lat_lon;
    double local_alt;

    if (scene_info.suffix()==QString("kml")){
        m_kml_handler.readFile(_scene_file_basename);
        scene_file = scene_info.absoluteDir().filePath(QString::fromStdString(m_kml_handler.getModelPath())).toStdString();
        local_lat_lon.setX(m_kml_handler.getModelLat());
        local_lat_lon.setY(m_kml_handler.getModelLon());
        local_alt = m_kml_handler.getModelAlt();
    }else{
        scene_file = _scene_file_basename;
        local_lat_lon.setX(0);
        local_lat_lon.setY(0);
        local_alt = 0;
    }

    std::string name = scene_file; //_scene_file_basename.substr(0, _scene_file_basename.find_last_of("."));

    // LOD processing
    osg::ref_ptr<SmartLOD> lodroot = new SmartLOD;
    lodroot->setDatabaseOptions(new osgDB::Options("noRotation"));

    std::string path0 = name;
    path0 = path0 + SmartLOD::EXTLOD0; // "-0.osgb";
    lodroot->addChild(path0, 0.0f, 40.0f);

    std::string path1 = name;
    path1 = path1 + SmartLOD::EXTLOD1; //"-1.osgb";
    lodroot->addChild(path1, 40.0f, 200.0f);

    std::string path2 = name;
    path2 = path2 + SmartLOD::EXTLOD2; //"-2.osgb";
    osg::ref_ptr<osg::Node> modelL2 =
            osgDB::readRefNodeFile(path2, new osgDB::Options("noRotation"));
    lodroot->addChild(modelL2.get(), 200.0f, FLT_MAX);
    lodroot->setFileName(2, path2);
    lodroot->doNotDiscardChild(2);

    // SmartLOD
    lodroot->setDatabaseOptions(new osgDB::Options("noRotation"));

    if (!modelL2)
    {
        std::cout << "No data loaded" << std::endl;
        return model_transform;
    }

    // Transform model
    model_transform = new osg::MatrixTransform;
    if (m_ref_alt == INVALID_VALUE){
        m_ref_lat_lon = local_lat_lon;
        m_ref_alt = local_alt;
        m_ltp_proj.Reset(m_ref_lat_lon.x(), m_ref_lat_lon.y(),m_ref_alt);

        osg::Matrix matrix = osg::Matrix::identity();
        model_transform->setMatrix(matrix);
    }else{
        double N,E,U;
        m_ltp_proj.Forward(local_lat_lon.x(), local_lat_lon.y(), local_alt, E, N, U);

        model_transform->setMatrix(osg::Matrix::translate(E,N,U));
    }

    model_transform->addChild(lodroot);

    return model_transform;
}


void OSGWidget::setCameraOnNode(osg::ref_ptr<osg::Node> _node)
{
    osgViewer::View *view = m_viewer->getView(0);

    view->setSceneData( m_matrixTransform); //m_group );
    // get the translation in the  node
    osg::MatrixTransform *matrix_transform = dynamic_cast <osg::MatrixTransform*> (_node.get());
    osg::Vec3d translation = matrix_transform->getMatrix().getTrans();
    BoxVisitor boxVisitor;
    _node->accept(boxVisitor);

    osg::BoundingBox box = boxVisitor.getBoundingBox();
    double x_max = box.xMax();
    double x_min = box.xMin();
    double y_max = box.yMax();
    double y_min = box.yMin();
    double cam_center_x = (x_max+x_min)/2 +  translation.x();
    double cam_center_y = (y_max+y_min)/2 +  translation.y();
    double cam_center_z;
    if( (x_max-x_min)/(2*tan(((30/2)* M_PI )/ 180.0 )) > (y_max-y_min)/(2*tan(((30* M_PI )/ 180.0 )/2)) )
    {
        cam_center_z = (x_max-x_min)/(2*tan(((30/2)* M_PI )/ 180.0 ));
    }
    else
    {
        cam_center_z = (y_max-y_min)/(2*tan(((30/2)* M_PI )/ 180.0 ));
    }

    osg::Vec3d eye(cam_center_x,
                   cam_center_y,
                   (box.zMin()*m_zScale + cam_center_z)); //*m_zScale);
    osg::Vec3d target( cam_center_x,
                       cam_center_y,
                       box.zMin()*m_zScale);
    osg::Vec3d normal(0,0,-1);

    view->getCameraManipulator()->setHomePosition(eye,target,normal);
}


///
/// \brief removeNodeFromScene remove a node from the scene
/// \param _node node to be removed
/// \return true if remove succeded
///
bool OSGWidget::removeNodeFromScene(osg::ref_ptr<osg::Node> _node)
{
    // remove model
    std::vector<osg::ref_ptr<osg::Node>>::iterator position = std::find(m_models.begin(), m_models.end(), _node);
    if (position != m_models.end()) // == myVector.end() means the element was not found
        m_models.erase(position);

    m_modelsGroup->removeChild(_node.get());

    // optimize the scene graph, remove redundant nodes and state etc.
    osgUtil::Optimizer optimizer;
    optimizer.optimize(this->m_globalGroup.get());

    osgViewer::View *view = m_viewer->getView(0);

    view->setSceneData( m_matrixTransform);

    update();

    return true;
}


void OSGWidget::setClearColor(double _r, double _g, double _b, double _alpha)
{
    std::vector<osg::Camera*> cameras;
    m_viewer->getCameras( cameras );

    for (unsigned int i=0; i<cameras.size(); i++){
        cameras[i]->setClearColor( osg::Vec4( _r, _g, _b, _alpha ));
    }

}

// To rewrite /////////////////////////////////////////////////////////////////////////////////////
void OSGWidget::clearSceneData()
{

    osgViewer::View *view = m_viewer->getView(0);
    view->setSceneData( 0 );
    view->getDatabasePager()->cancel();
    view->getDatabasePager()->clear();

    // remove all nodes from group
    for (unsigned int i=0; i<m_models.size(); i++){
        m_modelsGroup->removeChild(m_models[i]);
        //m_models[i] = NULL; useless
    }

    m_models.clear();

    // remove all drawables
    // m_measurement_geode->removeDrawables(0,m_measurement_geode->getNumDrawables());
    for (unsigned int i=0; i<m_geodes.size(); i++)
    {
        m_geodes[i]->removeDrawables(0,m_geodes[i]->getNumDrawables());
        m_geodesGroup->removeChild(m_geodes[i]);
        //m_models[i] = NULL; useless
    }
    m_geodes.clear();

    // reinit georef
    m_ref_lat_lon.setX(INVALID_VALUE);
    m_ref_lat_lon.setY(INVALID_VALUE);
    m_ref_alt = INVALID_VALUE;

    this->initializeGL();
    update();
}

void OSGWidget::initializeGL(){

    // Init properties
    osg::StateSet* state_set = m_modelsGroup->getOrCreateStateSet();
    osg::Material* material = new osg::Material;
    material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );
    state_set->setAttributeAndModes( material, osg::StateAttribute::ON );
    state_set->setMode(GL_BLEND, osg::StateAttribute::ON);
    state_set->setMode(GL_LINE_SMOOTH, osg::StateAttribute::OFF);
    state_set->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    // to show measures too
    state_set = m_geodesGroup->getOrCreateStateSet();
    // material needed to show colors in measure without light
    material = new osg::Material;
    material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );
    state_set->setAttributeAndModes( material, osg::StateAttribute::ON );
    state_set->setMode(GL_BLEND, osg::StateAttribute::ON);
    state_set->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    // if selected : only parts on top of all madels are shown
    //state_set->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );
}

void OSGWidget::paintGL()
{
    //qDebug() << "frame";

    m_viewer->frame();

    //paintOverlayGL();
}

void OSGWidget::paintOverlayGL()
{
    if(!m_showZScale)
    {
        m_overlay->hide();
        return;
    }

    QPainter painter(this);
    painter.beginNativePainting();
    painter.setViewTransformEnabled(false);
    painter.setWorldMatrixEnabled(false);

    QPen pen(Qt::gray, 1, Qt::SolidLine);
    painter.setPen(pen);
    QFont font = painter.font();
    font.setPixelSize(12);
    painter.setFont(font);

    float z_offset = m_ref_alt;
    if(m_ref_alt == INVALID_VALUE)
        z_offset = 0;

    float minval = m_modelsZMin + z_offset;
    float maxval = m_modelsZMax + z_offset;

    if(m_useDisplayZMinMax)
    {
        minval = m_displayZMin + z_offset;
        maxval = m_displayZMax + z_offset;
    }

    m_overlay->setMinMax(minval, maxval);
    m_overlay->setColorPalette(m_colorPalette);
    m_overlay->show();
    m_overlay->update();
}


void OSGWidget::resizeGL( int _width, int _height )
{
    this->getEventQueue()->windowResize( this->x(), this->y(), _width, _height );
    m_graphicsWindow->resized( this->x(), this->y(), _width, _height );

    this->onResize( _width, _height );
}

void OSGWidget::keyPressEvent( QKeyEvent* _event )
{
    QString key_string   = _event->text();
    const char* key_data = key_string.toLocal8Bit().data();

    if( _event->key() == Qt::Key_Control )
    {
        m_ctrl_pressed = true;
    }

    this->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KeySymbol( *key_data ) );
}

void OSGWidget::keyReleaseEvent( QKeyEvent* _event )
{
    QString key_string   = _event->text();
    const char* key_data = key_string.toLocal8Bit().data();

    if( _event->key() == Qt::Key_Control )
    {
        m_ctrl_pressed =  false;
    }

    this->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KeySymbol( *key_data ) );
}

void OSGWidget::mouseMoveEvent( QMouseEvent* _event )
{
    emit signal_onMouseMove(_event->x(), _event->y());

    this->getEventQueue()->mouseMotion( static_cast<float>( _event->x() ),
                                        static_cast<float>( _event->y() ) );
}

void OSGWidget::mousePressEvent( QMouseEvent* _event )
{

    // for tools
    if( m_ctrl_pressed == true && _event->button()==Qt::LeftButton)
    {
        emit signal_onMousePress(Qt::MiddleButton, _event->x(), _event->y());
    }
    else
        emit signal_onMousePress(_event->button(), _event->x(), _event->y());

    // 1 = left mouse button
    // 2 = middle mouse button
    // 3 = right mouse button

    unsigned int button = 0;

    switch( _event->button() )
    {
    case Qt::LeftButton:
    {

        if( m_ctrl_pressed == true)
        {
            button = 2;
            m_fake_middle_click_activated = true;
        }else{
            button = 1;
        }
    }
        break;

    case Qt::MiddleButton:
    {
        button = 2;
    }
        break;

    case Qt::RightButton:
    {
        button = 3;
    }
        break;

    default:
        break;
    }

    this->getEventQueue()->mouseButtonPress( static_cast<float>( _event->x() ),
                                             static_cast<float>( _event->y() ),
                                             button );
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

        //        if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
        //        {
        //            // the geodes are identified by name.
        //            std::cout<<"Object \""<<hitr->nodePath.back()->getName()<<"\""<<std::endl;
        //        }
        //        else if (hitr->drawable.valid())
        //        {
        //            std::cout<<"Object \""<<hitr->drawable->className()<<"\""<<std::endl;
        //        }


        // we get the intersections in a osg::Vec3d
        _inter_point = hitr->getWorldIntersectPoint();

        _inter_point[2] /= m_zScale;

    }else{
        _inter_exists = false;
    }

    if(!_inter_exists)
    {
        osgUtil::PolytopeIntersector::Intersections intersections;
        osgUtil::PolytopeIntersector *polyintersector =
                new osgUtil::PolytopeIntersector(osgUtil::Intersector::CoordinateFrame::WINDOW,_x-3,this->size().height() -_y-3,_x+3,this->size().height() - _y+3);
        polyintersector->setPrimitiveMask(osgUtil::PolytopeIntersector::POINT_PRIMITIVES);
        osgUtil::IntersectionVisitor iv(polyintersector);

        polyintersector->setIntersectionLimit(osgUtil::PolytopeIntersector::LIMIT_NEAREST);
        osg::Camera *cam = view->getCamera();
        cam->accept(iv);
        intersections = polyintersector->getIntersections();

        if(!intersections.empty())
        {
            _inter_exists = true;

            osgUtil::PolytopeIntersector::Intersections::iterator hitr = intersections.begin();

            // we get the intersections in a osg::Vec3d
            _inter_point = hitr->localIntersectionPoint;

            _inter_point[2] /= m_zScale;
        }
        else
        {
            _inter_exists = false;
        }
    }
}

void OSGWidget::getIntersectionPoint(osg::Vec3d _world_point, osg::Vec3d &_inter_point, bool &_inter_exists)
{
    // project point
    osgViewer::View *view = m_viewer->getView(0);
    osg::Camera *cam = view->getCamera();

    const osg::Matrixd transmat
            = cam->getViewMatrix()
            * cam->getProjectionMatrix()
            * cam->getViewport()->computeWindowMatrix();

    osg::Vec4d vec(_world_point[0], _world_point[1], _world_point[2], 1.0);

    vec = vec * transmat;
    vec = vec / vec.w();

    float x = vec.x();
    float y = vec.y();

    osgUtil::LineSegmentIntersector::Intersections intersections;


    if (view->computeIntersections(x, y, intersections))
    {
        _inter_exists = true;

        osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();

        // we get the intersections in a osg::Vec3d
        _inter_point = hitr->getWorldIntersectPoint();

        _inter_point[2] /= m_zScale;

    }else{
        _inter_exists = false;

        osgUtil::PolytopeIntersector::Intersections intersections;
        osgUtil::PolytopeIntersector *polyintersector =
                new osgUtil::PolytopeIntersector(osgUtil::Intersector::CoordinateFrame::WINDOW,x-3,y-3,x+3,y+3);
        polyintersector->setPrimitiveMask(osgUtil::PolytopeIntersector::POINT_PRIMITIVES);
        osgUtil::IntersectionVisitor iv(polyintersector);
        polyintersector->setIntersectionLimit(osgUtil::PolytopeIntersector::LIMIT_NEAREST);
        cam->accept(iv);
        intersections = polyintersector->getIntersections();

        if(!intersections.empty())
        {
            _inter_exists = true;

            osgUtil::PolytopeIntersector::Intersections::iterator hitr = intersections.begin();

            // we get the intersections in a osg::Vec3d
            _inter_point = hitr->localIntersectionPoint;

            _inter_point[2] /= m_zScale;
        }
    }
}

void OSGWidget::getIntersectionPointNode(int _x, int _y, osg::ref_ptr<osg::Node> &_inter_node,  bool &_inter_exists)
{
    osgUtil::PolytopeIntersector::Intersections intersections;
    osgUtil::PolytopeIntersector *polyintersector =
            new osgUtil::PolytopeIntersector(osgUtil::Intersector::CoordinateFrame::WINDOW,_x-3,this->size().height() -_y-3,_x+3,this->size().height() - _y+3);
    polyintersector->setPrimitiveMask(
                osgUtil::PolytopeIntersector::POINT_PRIMITIVES
                | osgUtil::PolytopeIntersector::LINE_PRIMITIVES);
    osgUtil::IntersectionVisitor iv(polyintersector);

    // do not work to restrict search
    //iv.apply(*m_geodesGroup);
    osgViewer::View *view = m_viewer->getView(0);

    polyintersector->setIntersectionLimit(osgUtil::PolytopeIntersector::LIMIT_NEAREST);
    osg::Camera *cam = view->getCamera();
    cam->accept(iv);
    intersections = polyintersector->getIntersections();

    m_geodesGroup->accept(iv);

    _inter_exists = false;

    if(!intersections.empty())
    {
        osgUtil::PolytopeIntersector::Intersections::iterator hitr = intersections.begin();

        while(hitr != intersections.end())
        {
            osg::ref_ptr<osg::Node> newnode = hitr->drawable->getParent(0);

            if(newnode != nullptr)
            {
                osg::Group *parent = newnode->getParent(0);
                if(parent != nullptr)
                {
                    std::string name = parent->getName();
                    if(name == MEASUREMENT_NAME)
                    {
                        _inter_exists = true;
                        _inter_node = newnode;
                        break;
                    }
                }

            }
            ++hitr;
        }
    }

}

void OSGWidget::mouseReleaseEvent(QMouseEvent* _event)
{

    // 1 = left mouse button
    // 2 = middle mouse button
    // 3 = right mouse button

    unsigned int button = 0;

    switch( _event->button() )
    {
    case Qt::LeftButton:
        if( m_fake_middle_click_activated == true)
        {
            button = 2;
            m_fake_middle_click_activated = false;
        }
        else
        {
            button = 1;
        }
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

    this->getEventQueue()->mouseButtonRelease( static_cast<float>( _event->x() ),
                                               static_cast<float>( _event->y() ),
                                               button );

}

void OSGWidget::wheelEvent( QWheelEvent* _event )
{

    _event->accept();
    int delta = _event->delta();

    // Inversion of wheel action : to be like in Google Maps
    // (just change test)
    osgGA::GUIEventAdapter::ScrollingMotion motion = delta < 0 ?   osgGA::GUIEventAdapter::SCROLL_UP
                                                                 : osgGA::GUIEventAdapter::SCROLL_DOWN;

    this->getEventQueue()->mouseScroll( motion );
}

bool OSGWidget::event( QEvent* _event )
{
    bool handled = QOpenGLWidget::event( _event );

    // This ensures that the OSG widget is always going to be repainted after the
    // user performed some interaction. Doing this in the event handler ensures
    // that we don't forget about some event and prevents duplicate code.
    switch( _event->type() )
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

void OSGWidget::onResize( int _width, int _height )
{
    std::vector<osg::Camera*> cameras;
    m_viewer->getCameras( cameras );

    cameras[0]->setViewport( 0, 0, _width, _height );
    //cameras[1]->setViewport( this->width() / 2, 0, this->width() / 2, this->height() );

    m_overlay->move(_width - m_overlay->width() - 10, _height - m_overlay->height()- 10);
}

osgGA::EventQueue* OSGWidget::getEventQueue() const
{
    osgGA::EventQueue* event_queue = m_graphicsWindow->getEventQueue();

    if( event_queue )
        return event_queue;
    else
        throw std::runtime_error( "Unable to obtain valid event queue");
}

void OSGWidget::getGeoOrigin(QPointF &_ref_lat_lon, double &_ref_alt)
{
    _ref_lat_lon = m_ref_lat_lon;
    _ref_alt = m_ref_alt;
}

// set initial values
void OSGWidget::setGeoOrigin(QPointF _latlon, double _alt)
{
    // Transform model
    osg::ref_ptr<osg::MatrixTransform> model_transform = new osg::MatrixTransform;
    m_ref_lat_lon = _latlon;
    m_ref_alt = _alt;
    m_ltp_proj.Reset(m_ref_lat_lon.x(), m_ref_lat_lon.y(),m_ref_alt);

    model_transform->setMatrix(osg::Matrix::identity()); //translate(0,0,0));
    osg::ref_ptr<osg::Geode> node = new osg::Geode();

    // hack : create an invisible point

    osg::Vec3d point;
    point[0] = m_ref_lat_lon.x();
    point[1] =  m_ref_lat_lon.y();
    point[2] = m_ref_alt;

    // create invisible point in geode
    // point
    osg::Geometry* shape_point_drawable = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array;
    vertices->push_back(point);

    // pass the created vertex array to the points geometry object.
    shape_point_drawable->setVertexArray(vertices);

    osg::Vec4Array* colors = new osg::Vec4Array;
    // add a white color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    osg::Vec4 color(0.0f,0.0f,0.0f,0.0f);
    colors->push_back(color);

    // pass the color array to points geometry, note the binding to tell the geometry
    // that only use one color for the whole object.
    shape_point_drawable->setColorArray(colors, osg::Array::BIND_OVERALL);

    // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
    // parameter passed to the DrawArrays constructor is the Primitive::Mode which
    // in this case is POINTS (which has the same value GL_POINTS), the second
    // parameter is the index position into the vertex array of the first point
    // to draw, and the third parameter is the number of points to draw.
    shape_point_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));

    // fixed size points
    shape_point_drawable->getOrCreateStateSet()->setAttribute(new osg::Point(4.f), osg::StateAttribute::ON);

    node->removeDrawables(0);
    node->addDrawable(shape_point_drawable);

    // end add invisible point

    model_transform->addChild(node);

    // Add model without userdata
    m_models.push_back(node);
    m_modelsGroup->insertChild(0, node.get()); // put at the beginning to be drawn first

    update();
}

void OSGWidget::addGeode(osg::ref_ptr<osg::Geode> _geode)
{
    m_geodesGroup->addChild(_geode.get());
    m_geodes.push_back(_geode);
    update();
}

void OSGWidget::removeGeode(osg::ref_ptr<osg::Geode> _geode)
{
    // remove geode
    std::vector<osg::ref_ptr<osg::Geode>>::iterator position = std::find(m_geodes.begin(), m_geodes.end(), _geode);
    if (position != m_geodes.end()) // == myVector.end() means the element was not found
        m_geodes.erase(position);

    m_geodesGroup->removeChild(_geode);
    update();
}

void OSGWidget::addGroup(osg::ref_ptr<osg::Group> _group)
{
    //m_groups.push_back(_group);
    m_geodesGroup->addChild(_group.get());
    update();
}

void OSGWidget::removeGroup(osg::ref_ptr<osg::Group> _group)
{
    // remove group
    //    std::vector<osg::ref_ptr<osg::Group>>::iterator position = std::find(m_groups.begin(), m_groups.end(), _group);
    //    if (position != m_groups.end()) // == myVector.end() means the element was not found
    //        m_groups.erase(position);

    m_geodesGroup->removeChild(_group);
    update();
}

// reset view to home
void OSGWidget::home()
{
    if(m_viewer == nullptr)
        return;

    osgViewer::View *view = m_viewer->getView(0);
    if(view)
        view->home();
    update();
}

// tools : emit correspondant signal
void OSGWidget::startTool(QString &_message)
{
    emit signal_startTool(_message);
}

void OSGWidget::endTool(QString &_message)
{
    emit signal_endTool(_message);
}

void OSGWidget::cancelTool(QString &_message)
{
    emit signal_cancelTool(_message);
}

bool OSGWidget::generateGeoAltitudeTiff(osg::ref_ptr<osg::Node> _node, QString _filename, double _pixel_size)
{
    std::string fileName = _filename.toStdString();
    ElevationMapCreator emc(m_ref_lat_lon, _pixel_size);
    bool status = emc.process(_node,fileName,this);
    return status;
}

bool OSGWidget::generateGeoOrthoTiff(osg::ref_ptr<osg::Node> _node, QString _filename, double _pixel_size)
{
    // get the translation in the  node
    osg::MatrixTransform *matrix_transform = dynamic_cast <osg::MatrixTransform*> (_node.get());
    osg::Vec3d translation = matrix_transform->getMatrix().getTrans();

    BoxVisitor boxVisitor;
    _node->accept(boxVisitor);

    osg::BoundingBox box = boxVisitor.getBoundingBox();

    // Create the edge of our picture
    // Set graphics contexts
    double x_max = box.xMax();
    double x_min = box.xMin();
    double y_max = box.yMax();
    double y_min = box.yMin();
    int width_pixel = ceil((x_max-x_min)/_pixel_size);
    int height_pixel = ceil((y_max-y_min)/_pixel_size);
    double width_meter = _pixel_size*width_pixel;
    double height_meter = _pixel_size*height_pixel;
    double cam_center_x = (x_max+x_min)/2 +  translation.x();
    double cam_center_y = (y_max+y_min)/2 +  translation.y();

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 0;
    traits->y = 0;
    traits->width = width_pixel;
    traits->height = height_pixel;
    traits->pbuffer = true;
    traits->alpha =  1;
    traits->sharedContext = 0;
    traits->doubleBuffer = false;
    traits->readDISPLAY();
    if(traits->displayNum < 0)
        traits->displayNum  = 0;
    traits->screenNum = 0;
    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());

    osg::ref_ptr< osg::Group > root( new osg::Group );
    root->addChild( _node );

    // setup MRT camera
    std::vector<osg::Texture2D*> attached_textures;
    osg::ref_ptr<osg::Camera> mrt_camera = new osg::Camera;
    mrt_camera->setGraphicsContext(gc);
    mrt_camera->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    mrt_camera->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    mrt_camera->setRenderOrder( osg::Camera::PRE_RENDER );
    mrt_camera->setViewport( 0, 0, width_pixel, height_pixel );
    mrt_camera->setClearColor(osg::Vec4(0., 0., 0., 0.));

    // Create our Texture
    osg::Texture2D* tex = new osg::Texture2D;
    tex->setTextureSize( width_pixel, height_pixel );
    tex->setSourceType( GL_UNSIGNED_BYTE );
    tex->setSourceFormat( GL_RGBA );
    tex->setInternalFormat( GL_RGBA32F_ARB );
    tex->setResizeNonPowerOfTwoHint( false );
    tex->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR );
    tex->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );
    attached_textures.push_back( tex );
    mrt_camera->attach( osg::Camera::COLOR_BUFFER, tex );

    // set RTT textures to quad
    osg::Geode* geode( new osg::Geode );
    geode->addDrawable( osg::createTexturedQuadGeometry(
                            osg::Vec3(-1,-1,0), osg::Vec3(2.0,0.0,0.0), osg::Vec3(0.0,2.0,0.0)) );
    geode->getOrCreateStateSet()->setTextureAttributeAndModes( 0, attached_textures[0] );
    geode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    //geode->getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    // configure postRenderCamera to draw fullscreen textured quad
    osg::Camera* post_render_camera( new osg::Camera );
    post_render_camera->setClearMask( 0 );
    post_render_camera->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER, osg::Camera::FRAME_BUFFER );
    post_render_camera->setReferenceFrame( osg::Camera::ABSOLUTE_RF );
    post_render_camera->setRenderOrder( osg::Camera::POST_RENDER );
    post_render_camera->setViewMatrix( osg::Matrixd::identity() );
    post_render_camera->setProjectionMatrix( osg::Matrixd::identity() );

    post_render_camera->addChild( geode );

    root->addChild(post_render_camera);

    // Create the viewer
    osgViewer::Viewer viewer;
    viewer.setThreadingModel( osgViewer::Viewer::SingleThreaded );
    viewer.setRunFrameScheme( osgViewer::ViewerBase::ON_DEMAND );

    viewer.setCamera( mrt_camera.get() );
    viewer.getCamera()->setProjectionMatrixAsOrtho2D(-width_meter/2,width_meter/2,-height_meter/2,height_meter/2);

    // put our model in the center of our viewer
    viewer.setCameraManipulator(new osgGA::TrackballManipulator());
    double cam_center_z= (x_max-x_min)/2 + (y_max-y_min)/2 ;


    osg::Vec3d eyes(cam_center_x,
                    cam_center_y,
                    box.zMin() + cam_center_z);
    osg::Vec3d center(cam_center_x,
                      cam_center_y,
                      box.zMin());
    osg::Vec3d normal(0,0,-1);
    viewer.getCameraManipulator()->setHomePosition(eyes,center,normal);

    viewer.setSceneData( root.get() );
    viewer.realize();

    // setup the callback
    osg::BoundingBox image_bounds;
    image_bounds.xMin() = cam_center_x-width_meter/2;
    image_bounds.xMax() = cam_center_x+width_meter/2;
    image_bounds.yMin() = cam_center_y-height_meter/2;
    image_bounds.yMax() = cam_center_y+height_meter/2;

    //    osgViewer::Viewer::Windows ws;
    //    // Get the window
    //    viewer.getWindows(ws);
    //    if (!ws.empty())
    //    {
    //        osgViewer::Viewer::Windows::iterator iter = ws.begin();
    //        (*iter)->setWindowRectangle(0, 0, width_pixel, height_pixel);
    //    }

    std::string screen_capture_filename = _filename.toStdString();
    bool hasShader = isEnabledShaderOnNode(_node);
    enableShaderOnNode(_node, false);

    SnapGeotiffImage* final_draw_callback = new SnapGeotiffImage(screen_capture_filename,m_ref_lat_lon, image_bounds,_pixel_size, this);
    mrt_camera->setFinalDrawCallback(final_draw_callback);

    viewer.home();
    viewer.frame();

    bool status = final_draw_callback->status();

    mrt_camera->removeFinalDrawCallback(final_draw_callback);

    // causes SEGV
    //delete final_draw_callback;

    viewer.setSceneData(nullptr);

    enableShaderOnNode(_node, hasShader);

    return status;
}

// convert x, y, z => lat, lon & alt
// if(m_ref_alt == INVALID_VALUE) do nothing
void OSGWidget::xyzToLatLonAlt(double _x, double _y, double _z, double &_lat, double &_lon, double &_alt)
{
    if(m_ref_alt == INVALID_VALUE)
        return;

    m_ltp_proj.Reverse(_x, _y, _z, _lat, _lon, _alt);
}

void OSGWidget::enableLight(bool _state)
{
    bool lighton = true;
    osg::StateAttribute::Values light =  osg::StateAttribute::OFF;

    if ( _state )
    {
        light =  osg::StateAttribute::ON;
        m_viewer->getView(0)->getCamera()->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
        // disable shades on shader
        lighton = false;
    }
    else
    {
        light =  osg::StateAttribute::OFF;
        m_viewer->getView(0)->getCamera()->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        // enable shades on shader
        lighton = true;
    }

    for(unsigned int i=0; i<m_models.size(); i++)
    {
        osg::StateSet* state_set = m_models[i]->getOrCreateStateSet();
        state_set->addUniform( new osg::Uniform( "lighton", lighton));
    }
}

void OSGWidget::enableStereo(bool _state)
{
    //osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::VERTICAL_INTERLACE);
    osg::DisplaySettings::instance()->setStereo(_state);
}

void OSGWidget::setNodeTransparency(osg::ref_ptr<osg::Node> _node, double _transparency_value)
{
    osg::StateSet* state_set = _node->getOrCreateStateSet();
    osg::StateAttribute* attr = state_set->getAttribute(osg::StateAttribute::MATERIAL);
    osg::Material* material = dynamic_cast<osg::Material*>(attr);

    state_set->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    if(_transparency_value == 0.0)
    {
        state_set->removeAttribute(osg::StateAttribute::MATERIAL);
        state_set->setMode( GL_BLEND, osg::StateAttribute::OFF);
    }
    else
    {
        state_set->setMode( GL_BLEND, osg::StateAttribute::ON);

        if(material == nullptr)
        {
            // Add the possibility of modifying the transparence
            material = new osg::Material;
            // Put the 3D model totally opaque
            material->setAlpha( osg::Material::FRONT, _transparency_value);
            state_set->setAttributeAndModes ( material, osg::StateAttribute::ON );
        }

        // Changes the transparency of the node
        material->setAlpha(osg::Material::FRONT, _transparency_value );

        // Turn on blending
        osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::ONE_MINUS_SRC_ALPHA,osg::BlendFunc::SRC_ALPHA );
        state_set->setAttributeAndModes(bf);

        state_set->setAttributeAndModes( material, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    }

    // alpha on shader
    state_set->addUniform( new osg::Uniform( "alpha", float(_transparency_value) ));

    update();
    //    // test
    //    osg::StateSet* state_set = _node->getOrCreateStateSet();
    //    osg::StateAttribute* attr = state_set->getAttribute(osg::StateAttribute::MATERIAL);
    //    osg::Material* material = dynamic_cast<osg::Material*>(attr);
    //    material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, _transparency_value) );
    //    state_set->setAttributeAndModes( material, osg::StateAttribute::OVERRIDE);
}

void OSGWidget::setNodeTranslationOffset(double _offset_x, double _offset_y, double _offset_z, osg::ref_ptr<osg::Node> _node, osg::Vec3d _trans)
{
    osg::ref_ptr<osg::MatrixTransform> model_transform =  dynamic_cast<osg::MatrixTransform*>(_node.get());

    osg::Matrix matrix = osg::Matrix::translate(_trans.x() + _offset_x, _trans.y() + _offset_y, _trans.z() + _offset_z);

    model_transform->setMatrix(matrix);

    // for shaders
    osg::ref_ptr<NodeUserData> data = (NodeUserData*)(_node->getUserData());
    if(data != nullptr)
    {
        data->zoffset = (float)_offset_z;
    }

    recomputeGlobalZMinMax();

    update();
}

void OSGWidget::setZScale(double _newValue)
{

    //    osgViewer::View *view = m_viewer->getView(0);
    //    osg::Vec3d eye1, center1, up1;
    //    osgGA::CameraManipulator *man = view->getCameraManipulator();
    //    man->getHomePosition(eye1,center1, up1);

    //    osg::Matrixd matrix = man->getMatrix();

    // change
    //double oldScale = m_zScale;
    m_zScale = _newValue;

    m_matrixTransform->setMatrix(osg::Matrix::scale(1.0, 1.0, m_zScale));
    if(m_models.size() > 0)
    {
        for(int i=0; i<m_models.size(); i++)
        {
            osg::MatrixTransform *matrix_transform = dynamic_cast <osg::MatrixTransform*> (m_models[i].get());
            if(matrix_transform != nullptr)
            {
                setCameraOnNode(m_models[i]);
                break;
            }
        }
    }

    //view->getCameraManipulator()->setHomePosition(eye,target,normal);
    home();

    //    matrix.ptr()[14] *= m_zScale / oldScale;
    //    view->getCameraManipulator()->setByMatrix(matrix);
}

void OSGWidget::configureShaders( osg::StateSet* stateSet )
{  
    osg::Program* program = new osg::Program;
    program->setName("3dMetricsShader");
    program->addShader( ShaderBuilder::fragmentShader(ShaderBuilder::Standard) );
    program->addShader( ShaderBuilder::vertexShader(ShaderBuilder::Standard, m_colorPalette) );

    stateSet->setAttribute( program, osg::StateAttribute::ON );

    stateSet->addUniform( new osg::Uniform( "alpha", 1.0f));
    stateSet->addUniform( new osg::Uniform( "pointsize", 32.0f));

//    // test EDL
//    osg::ref_ptr<osg::Image> image = new osg::Image();
//    image->allocateImage(width(), height(), 1, GL_RGBA, GL_UNSIGNED_BYTE);
//    osg::Vec4 color(0.5,0.6,0.7,1.0);
//    for(int i=0; i<width(); i++)
//    {
//        for(int j=0; j<height(); j++)
//        {
//            image->setColor(color,i,j);
//        }
//    }
//    osg::ref_ptr<osg::Texture2D> tex(new osg::Texture2D());               // (1)

//      tex->setImage(image);
//      stateSet->setTextureAttributeAndModes(0, tex);
//      osg::ref_ptr<osg::TexGen> texGen(new osg::TexGen());                // (2)
//        texGen->setPlane(osg::TexGen::S, osg::Plane(0.075, 0.0, 0.0, 0.5)); // (2)
//        texGen->setPlane(osg::TexGen::T, osg::Plane(0.0, 0.035, 0.0, 0.3)); // (2)
//        stateSet->setTextureAttributeAndModes(0, texGen);                         // (2)

//    stateSet->addUniform( new osg::Uniform( "s1_color", 1));
//    stateSet->addUniform( new osg::Uniform( "s2_depth", 0));
//    stateSet->addUniform( new osg::Uniform( "Pix_scale", 1));
//    stateSet->addUniform( new osg::Uniform( "Exp_scale", 1.0f));

//    osg::Uniform *neighbors = new osg::Uniform(osg::Uniform::FLOAT_VEC2,"Neigh_pos_2D", 8);
//    for (unsigned c = 0; c < 8; c++)
//    {
//        osg::Vec2 neib;

//        neib.x() = std::cos(c * M_PI / 4.0);
//        neib.y() = std::sin(c * M_PI / 4.0);
//        neighbors->setElement(c,neib);
//    }
//    stateSet->addUniform( neighbors);
//    stateSet->addUniform( new osg::Uniform( "Sx", (float)width()));
//    stateSet->addUniform( new osg::Uniform( "Sy", (float)height()));
//    stateSet->addUniform( new osg::Uniform( "Light_dir", osg::Vec3f(0.0f,1.0f,0.0f)));
//    // end test

    bool lighton = (m_viewer->getView(0)->getCamera()->getOrCreateStateSet()->getMode(GL_LIGHTING) == osg::StateAttribute::OFF);

    stateSet->addUniform( new osg::Uniform( "lighton", lighton));
    stateSet->setMode(GL_VERTEX_PROGRAM_POINT_SIZE, osg::StateAttribute::ON);

    update();
}


// recompute global zmin and zmax for all models
void OSGWidget::recomputeGlobalZMinMax()
{
    m_modelsZMin = 0;
    m_modelsZMax = 0;

    if(m_models.size() == 0)
    {
        return;
    }

    bool first = true;

    for(unsigned int i=0; i<m_models.size(); i++)
    {
        osg::ref_ptr<NodeUserData> data = (NodeUserData*)m_models[i]->getUserData();
        if(data != nullptr)
        {
            if(first)
            {
                m_modelsZMin = data->zmin + data->zoffset + data->originalZoffset;
                m_modelsZMax = data->zmax + data->zoffset + data->originalZoffset;
                first = false;
                continue;
            }

            float zmin = data->zmin + data->zoffset + data->originalZoffset;
            float zmax = data->zmax + data->zoffset + data->originalZoffset;

            if(zmin < m_modelsZMin)
                m_modelsZMin = zmin;

            if(zmax > m_modelsZMax)
                m_modelsZMax = zmax;
        }
    }

    if(first)
    {
        // no 3D models loaded
        return;
    }
    float delta = m_modelsZMax - m_modelsZMin;
    float min = m_modelsZMin;
    if(m_useDisplayZMinMax)
    {
        delta = m_displayZMax - m_displayZMin;
        min = m_displayZMin;
    }

    for(unsigned int i=0; i<m_models.size(); i++)
    {
        osg::ref_ptr<NodeUserData> data = (NodeUserData*)m_models[i]->getUserData();

        if(data == nullptr)
            continue;

        osg::StateSet* state_set = m_models[i]->getOrCreateStateSet();
        state_set->addUniform( new osg::Uniform( "zmin", min - data->zoffset - data->originalZoffset));
        state_set->addUniform( new osg::Uniform( "deltaz", delta));
    }
}

bool OSGWidget::isEnabledShaderOnNode(osg::ref_ptr<osg::Node> _node)
{
    osg::ref_ptr<NodeUserData> data = (NodeUserData*)(_node->getUserData());
    if(data != nullptr)
    {
        return data->useShader;
    }
    return false;
}

void OSGWidget::enableShaderOnNode(osg::ref_ptr<osg::Node> _node, bool _enable, bool _update)
{
    osg::ref_ptr<NodeUserData> data = (NodeUserData*)(_node->getUserData());
    if(data != nullptr)
    {
        osg::StateSet *stateSet= _node->getOrCreateStateSet();
        data->useShader = _enable;
        if(_enable)
        {
            configureShaders(stateSet);
        }
        else
        {
            stateSet->removeAttribute(osg::StateAttribute::PROGRAM);
        }
    }
    //m_viewer->setRunFrameScheme( osgViewer::ViewerBase::ON_DEMAND );
    if(_update)
        update();
}


void OSGWidget::setUseDisplayZMinMaxAndUpdate(bool _use)
{
    m_useDisplayZMinMax = _use;

    float delta = m_modelsZMax - m_modelsZMin;
    float min = m_modelsZMin;
    if(m_useDisplayZMinMax)
    {
        delta = m_displayZMax - m_displayZMin;
        min = m_displayZMin;
    }

    for(unsigned int i=0; i<m_models.size(); i++)
    {
        osg::ref_ptr<NodeUserData> data = (NodeUserData*)m_models[i]->getUserData();

        if(data == nullptr)
            continue;

        osg::StateSet* state_set = m_models[i]->getOrCreateStateSet();
        state_set->addUniform( new osg::Uniform( "zmin", min - data->zoffset - data->originalZoffset));
        state_set->addUniform( new osg::Uniform( "deltaz", delta));
    }

    update();
}

void OSGWidget::showZScale(bool _show)
{
    m_showZScale = _show;
    if(_show)
    {
        m_overlay->show();
        paintOverlayGL();
    }
    else
    {
        m_overlay->hide();
    }
    update();
}

void OSGWidget::setColorPalette(ShaderColor::Palette _palette)
{
    m_colorPalette = _palette;

    // process all models
    for(unsigned int i=0; i<m_models.size(); i++)
    {
        osg::ref_ptr<NodeUserData> data = (NodeUserData*)m_models[i]->getUserData();
        if(data != nullptr)
        {
            if(data->useShader)
            {
                osg::StateSet *stateSet= m_models[i]->getOrCreateStateSet();
                configureShaders(stateSet);

                // correct transparency if using shaders
                osg::StateSet* state_set = m_models[i]->getOrCreateStateSet();
                osg::StateAttribute* attr = state_set->getAttribute(osg::StateAttribute::MATERIAL);
                double transp = 0;
                if(attr != nullptr)
                {
                    osg::Material* material = dynamic_cast<osg::Material*>(attr);
                    if(material != nullptr)
                    {
                        osg::Vec4 color= material->getDiffuse(osg::Material::FRONT);
                        transp = color.a();
                    }
                }

                setNodeTransparency(m_models[i], transp);
            }
        }
    }
    showZScale(m_showZScale);
    update();
}
