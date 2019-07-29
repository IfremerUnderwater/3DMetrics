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
#include <osg/Point>
#include <osg/LineWidth>

#include <osg/PolygonMode>
#include <QProcess>
#include <math.h>
#include <limits>

#include <osg/AlphaFunc>
#include <osg/BlendFunc>

#include "Measurement/box_visitor.h"
#include "edit_transparency_model.h"

class  Transparency : public osg::NodeVisitor {

public :

    Transparency() : NodeVisitor( NodeVisitor::TRAVERSE_ALL_CHILDREN ) {}

    void apply ( osg::Node &node ){
    //osg::StateSet* state_set = _node->getOrCreateStateSet();
    /*osg::StateAttribute* attr = state_set->getAttribute(osg::StateAttribute::MATERIAL);
    osg::Material* material = dynamic_cast<osg::Material*>(attr);*/
        /*osg::StateSet* state_set = node.getOrCreateStateSet();
        osg::StateAttribute* attr = state_set->getAttribute(osg::StateAttribute::MATERIAL);
        osg::Material* material = dynamic_cast<osg::Material*>(attr);
        float alpha = (float)20/100;*/
        //material->setAlpha( osg::Material::FRONT_AND_BACK, alpha );

       //if (node.getStateSet()) apply(*node.getStateSet());
        traverse(node);
    } // apply( osg::Node &node )

    void apply(osg::Geode &geode)
    {
        /*osg::Drawable* drawable = geode.asDrawable();
        osg::StateSet* state_set = drawable->getStateSet();
        osg::StateAttribute* attr = state_set->getAttribute(osg::StateAttribute::MATERIAL);
        if (attr)
        {
            osg::Material* material = dynamic_cast<osg::Material*>(attr);
            float alpha = (float)20/100;
            material->setAlpha( osg::Material::FRONT_AND_BACK, alpha );
        }*/
        //if (geode.getStateSet()) apply(*geode.getStateSet());

        osg::StateSet* state_set = geode.getOrCreateStateSet();
        //osg::StateAttribute* attr = state_set->getAttribute(osg::StateAttribute::TEXTURE);
        osg::StateAttribute* attr = state_set->getAttribute(osg::StateAttribute::MATERIAL);
        state_set->setMode( GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        osg::ref_ptr<osg::Material> material;
        if(attr == NULL) {
            material = new osg::Material;
            //material->setTransparency(osg::Material::FRONT_AND_BACK,m_alpha);
            material->setAlpha( osg::Material::FRONT_AND_BACK, m_alpha );

            state_set->setAttributeAndModes ( material,osg::StateAttribute::ON );

            //osg::ref_ptr<osg::BlendFunc> bft = new osg::BlendFunc(osg::BlendFunc::DST_COLOR, osg::BlendFunc::SRC_ALPHA_SATURATE );
            //osg::ref_ptr<osg::BlendFunc> bft = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
            //state_set->setAttributeAndModes(bft);
            //osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
            //osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::SRC_COLOR, osg::BlendFunc::ONE_MINUS_SRC_COLOR );
            //state_set->setAttributeAndModes(bf);

        }
        else {
            material = dynamic_cast<osg::Material*>(attr);
            material->setAlpha( osg::Material::FRONT_AND_BACK, m_alpha );
            //material->setTransparency(osg::Material::FRONT_AND_BACK,m_alpha);
        }




       // osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::SRC_COLOR, osg::BlendFunc::ONE_MINUS_SRC_COLOR );
        //state_set->setAttributeAndModes(bf);

        state_set->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        /*osg::StateAttribute* attr = state_set->getAttribute(osg::StateAttribute::MATERIAL);
        osg::Material* material = dynamic_cast<osg::Material*>(attr);
        float alpha = (float)20/100;
        material->setAlpha( osg::Material::FRONT_AND_BACK, alpha );*/
    }

     void apply(osg::StateSet& stateset)
    {
         //osg::StateSet* state_set = geode.getOrCreateStateSet();
         stateset.setMode( GL_BLEND, osg::StateAttribute::ON );
         osg::ref_ptr<osg::Material> material = new osg::Material;
         material->setAlpha( osg::Material::FRONT_AND_BACK, 0.2 );

         stateset.setAttributeAndModes ( material.get(),osg::StateAttribute::ON );
         osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
         stateset.setAttributeAndModes(bf);
         /*stateset->setMode( GL_BLEND, osg::StateAttribute::ON );
        osg::ref_ptr<osg::Material> material = new osg::Material;
        material->setAlpha( osg::Material::FRONT_AND_BACK, 1 );

        stateset->setAttributeAndModes ( material.get(),osg::StateAttribute::ON );

        osg::StateAttribute* attr = stateset.getAttribute(osg::StateAttribute::MATERIAL);
        if (attr)
        {

            osg::Material* material = dynamic_cast<osg::Material*>(attr);
            float alpha = (float)20/100;
            material->setAlpha( osg::Material::FRONT_AND_BACK, alpha );*/
            /*stateset.setAttributeAndModes ( material,osg::StateAttribute::ON );
            osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
            stateset.setAttributeAndModes(bf);
        }*/
    }

    void setAlpha(float _alpha) { m_alpha = _alpha;}
    private :
     float m_alpha;
};

struct SnapImage : public osg::Camera::DrawCallback {
    SnapImage(osg::GraphicsContext* _gc,const std::string& _filename, QPointF &_ref_lat_lon,osg::BoundingBox _box, double _pixel_size) :
        m_filename( _filename ),
        m_ref_lat_lon( _ref_lat_lon ),
        m_box( _box ),
        m_pixel_size( _pixel_size )
    {
        m_image = new osg::Image;
        if (_gc->getTraits()) {
            int width = _gc->getTraits()->width;
            int height = _gc->getTraits()->height;
            m_image->allocateImage(width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
        }
    }

    virtual void operator () (osg::RenderInfo& renderInfo) const {
        osg::Camera* camera = renderInfo.getCurrentCamera();

        osg::GraphicsContext* gc = camera->getGraphicsContext();
        if (gc->getTraits() && m_image.valid()) {

            // get the image
            int width = gc->getTraits()->width;
            int height = gc->getTraits()->height;
            m_image->readPixels( 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE );

            // Variable for the command line "gdal_translate"
            double lat_0  = m_ref_lat_lon.x();
            double lon_0 = m_ref_lat_lon.y();
            double x_min = m_box.xMin();
            double y_max = m_box.yMax();

            std::string tiff_name = m_filename+".tif";
            GDALAllRegister();
            CPLPushErrorHandler(CPLQuietErrorHandler);
            GDALDataset *geotiff_dataset;
            GDALDriver *driver_geotiff;

            driver_geotiff = GetGDALDriverManager()->GetDriverByName("GTiff");
            geotiff_dataset = driver_geotiff->Create(tiff_name.c_str(),width,height,4,GDT_Byte,NULL);

            int size = height*width;
            for(int i=0; i<height; i++) {
                unsigned char buffer_R[width];
                unsigned char buffer_G[width];
                unsigned char buffer_B[width];
                unsigned char buffer_A[width];
                for(int j=0; j<(width); j++) {
                    buffer_R[width-j] = m_image->data(size - ((width*i)+j))[0];
                    buffer_G[width-j] = m_image->data(size - ((width*i)+j))[1];
                    buffer_B[width-j] = m_image->data(size - ((width*i)+j))[2];
                    buffer_A[width-j] = m_image->data(size - ((width*i)+j))[3];

                }
                // CPLErr GDALRasterBand::RasterIO( GDALRWFlag eRWFlag, int nXOff, int nYOff, int nXSize, int nYSize, void * pData, int nBufXSize, int nBufYSize, GDALDataType eBufType, int nPixelSpace, int nLineSpace )

                geotiff_dataset->GetRasterBand(1)->RasterIO(GF_Write,0,i,width,1,buffer_R,width,1,GDT_Byte,0,0);
                geotiff_dataset->GetRasterBand(2)->RasterIO(GF_Write,0,i,width,1,buffer_G,width,1,GDT_Byte,0,0);
                geotiff_dataset->GetRasterBand(3)->RasterIO(GF_Write,0,i,width,1,buffer_B,width,1,GDT_Byte,0,0);
                geotiff_dataset->GetRasterBand(4)->RasterIO(GF_Write,0,i,width,1,buffer_A,width,1,GDT_Byte,0,0);
            }

            // Setup output coordinate system.
            double geo_transform[6] = { x_min, m_pixel_size, 0, y_max, 0, -m_pixel_size };
            geotiff_dataset->SetGeoTransform(geo_transform);
            char *geo_reference = NULL;
            OGRSpatialReference o_SRS;
            o_SRS.SetTM(lat_0,lon_0,0.9996,0,0);
            o_SRS.SetWellKnownGeogCS( "WGS84" );
            o_SRS.exportToWkt( &geo_reference );

            geotiff_dataset->SetProjection(geo_reference);
            CPLFree( geo_reference );
            GDALClose(geotiff_dataset) ;

            GDALDestroyDriverManager();
        }
    }

    std::string m_filename;
    osg::ref_ptr<osg::Image> m_image;
    QPointF m_ref_lat_lon;
    osg::BoundingBox m_box;
    double m_pixel_size;
};

class KeyboardEventHandler : public osgGA::GUIEventHandler
{
public:

    KeyboardEventHandler(osg::StateSet* stateset):
        _stateset(stateset)
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
                if (_stateset->getMode(GL_LIGHTING) == osg::StateAttribute::OFF)
                    _stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
                else
                    _stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
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



OSGWidget::OSGWidget(QWidget* parent)
    : QOpenGLWidget( parent)
    , m_graphicsWindow( new osgViewer::GraphicsWindowEmbedded( this->x(),
                                                               this->y(),
                                                               this->width(),
                                                               this->height() ) )
    , m_viewer( new osgViewer::CompositeViewer )
    , m_ctrl_pressed(false)
    , m_fake_middle_click_activated(false)
    , m_material( new osg::Material )

{

    m_ref_lat_lon.setX(INVALID_VALUE);
    m_ref_lat_lon.setY(INVALID_VALUE);
    m_ref_depth = INVALID_VALUE;

    //osgDB::Registry::instance()->setLibraryFilePathList("/Users/tim/code/3DMetricWorkspace/Run/Release/3DMetrics.app/Contents/osgPlugins");

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
    view->addEventHandler(new KeyboardEventHandler(view->getCamera()->getOrCreateStateSet()));


    osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
    manipulator->setAllowThrow( false );

    view->setCameraManipulator( manipulator );

    m_viewer->addView( view );
    m_viewer->setThreadingModel( osgViewer::CompositeViewer::SingleThreaded );
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

    connect( &m_timer, SIGNAL(timeout()), this, SLOT(update()) );
    m_timer.start( 10 );

    // Create group that will contain measurement geode and 3D model
    m_group = new osg::Group;
    //    m_measurement_geode = new osg::Geode;
    //    m_group->addChild(m_measurement_geode);
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

    /***    // load the data
    setlocale(LC_ALL, "C");

    QFileInfo sceneInfo(QString::fromStdString(_sceneFile));
    std::string sceneFile;

    QPointF local_lat_lon;
    double local_depth;

    if (sceneInfo.suffix()==QString("kml")){
        m_kml_handler.readFile(_sceneFile);
        sceneFile = sceneInfo.absoluteDir().filePath(QString::fromStdString(m_kml_handler.getModelPath())).toStdString();
        local_lat_lon.setX(m_kml_handler.getModelLat());
        local_lat_lon.setY(m_kml_handler.getModelLon());
        local_depth = m_kml_handler.getModelAlt();
    }else{
        sceneFile = _sceneFile;
        local_lat_lon.setX(0);
        local_lat_lon.setY(0);
        local_depth = 0;
    }

    osg::ref_ptr<osg::Node> model_node=osgDB::readRefNodeFile(sceneFile, new osgDB::Options("noRotation"));

    if (!model_node)
    {
        std::cout << "No data loaded" << std::endl;
        return false;
    }

    // Transform model
    osg::ref_ptr<osg::MatrixTransform> model_transform = new osg::MatrixTransform;
    if (m_ref_depth == INVALID_VALUE){
        m_ref_lat_lon = local_lat_lon;
        m_ref_depth = local_depth;
        m_ltp_proj.Reset(m_ref_lat_lon.x(), m_ref_lat_lon.y(),m_ref_depth);


        model_transform->setMatrix(osg::Matrix::identity()); //translate(0,0,0));
        model_transform->addChild(model_node);
    }else{
        double N,E,U;
        m_ltp_proj.Forward(local_lat_lon.x(), local_lat_lon.y(), local_depth, E, N, U);

        model_transform->setMatrix(osg::Matrix::translate(E,N,U));
        //model_transform->setMatrix(osg::Matrix::translate(N,-U,E));
        model_transform->addChild(model_node);
    }

    // Add model
    m_models.push_back(model_transform);
    osg::StateSet* stateSet = model_transform->getOrCreateStateSet();
    stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    m_group->addChild(model_transform.get());

    // optimize the scene graph, remove redundant nodes and state etc.
    //osgUtil::Optimizer optimizer;
    //optimizer.optimize(m_group.get());

    osgViewer::View *view = m_viewer->getView(0);

    view->setSceneData( m_group );

    return true;
***/
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
    setlocale(LC_ALL, "C");

    QFileInfo scene_info(QString::fromStdString(_scene_file));
    std::string scene_file;

    QPointF local_lat_lon;
    double local_depth;

    if (scene_info.suffix()==QString("kml")){
        m_kml_handler.readFile(_scene_file);
        scene_file = scene_info.absoluteDir().filePath(QString::fromStdString(m_kml_handler.getModelPath())).toStdString();
        local_lat_lon.setX(m_kml_handler.getModelLat());
        local_lat_lon.setY(m_kml_handler.getModelLon());
        local_depth = m_kml_handler.getModelAlt();
    }else{
        scene_file = _scene_file;
        local_lat_lon.setX(0);
        local_lat_lon.setY(0);
        local_depth = 0;
    }

    osg::ref_ptr<osg::Node> model_node=osgDB::readRefNodeFile(scene_file, new osgDB::Options("noRotation"));

    if (!model_node)
    {
        std::cout << "No data loaded" << std::endl;
        return model_transform;
    }

    // Transform model
    model_transform = new osg::MatrixTransform;
    if (m_ref_depth == INVALID_VALUE){
        m_ref_lat_lon = local_lat_lon;
        m_ref_depth = local_depth;
        m_ltp_proj.Reset(m_ref_lat_lon.x(), m_ref_lat_lon.y(),m_ref_depth);


        model_transform->setMatrix(osg::Matrix::identity()); //translate(0,0,0));
        model_transform->addChild(model_node);
    }else{
        double N,E,U;
        m_ltp_proj.Forward(local_lat_lon.x(), local_lat_lon.y(), local_depth, E, N, U);

        model_transform->setMatrix(osg::Matrix::translate(E,N,U));
        //model_transform->setMatrix(osg::Matrix::translate(N,-U,E));
        model_transform->addChild(model_node);
    }

    return model_transform;
}

///
/// \brief addNodeToScene add a node to the scene
/// \param _node node to be added
/// \return true if loading succeded
///
bool OSGWidget::addNodeToScene(osg::ref_ptr<osg::Node> _node)
{
    // Add model
    m_models.push_back(_node);
    osg::StateSet* state_set = _node->getOrCreateStateSet();
    state_set->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    state_set->setMode( GL_BLEND, osg::StateAttribute::ON );
    osg::ref_ptr<osg::Material> material = new osg::Material;
    material->setAlpha( osg::Material::FRONT_AND_BACK, 1 );

    state_set->setAttributeAndModes ( material,osg::StateAttribute::ON );
    /*osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
    state_set->setAttributeAndModes(bf);*/

    //m_group->addChild(_node.get());
    m_group->insertChild(0, _node.get()); // put at the beginning to be drawn first

    // optimize the scene graph, remove redundant nodes and state etc.
    /*osgUtil::Optimizer optimizer;
    optimizer.optimize(m_group.get());*/

    osgViewer::View *view = m_viewer->getView(0);

    view->setSceneData( m_group );
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
                   box.zMin() + cam_center_z);
    osg::Vec3d target( cam_center_x,
                       cam_center_y,
                       box.zMin());
    osg::Vec3d normal(0,0,-1);

    view->getCameraManipulator()->setHomePosition(eye,target,normal);

    home();
    //state_set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   // state_set->setMode( GL_BLEND, osg::StateAttribute::ON );
    //state_set->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    /*osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
    alphaFunc->setFunction(osg::AlphaFunc::GREATER,0.2f);*/
    //osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
    //blendFunc->setFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    //state_set->setAttributeAndModes( blendFunc, osg::StateAttribute::ON );

    //Make sure blending is on.
    /*state_set->setMode( GL_BLEND,osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
    //Get the material if it exists
    osg::ref_ptr<osg::Material> material = new osg::Material;


    material->setAlpha( osg::Material::FRONT_AND_BACK, 0.5 );

    //if ( m_fAlpha >= 1.0f ){
    //Entity is opaque so turn off state attribute
    //stateSet->setAttributeAndModes( material,osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
    //}else{
    //Entity has transparency
    state_set->setAttributeAndModes( material.get(),osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
    osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
    state_set->setAttributeAndModes(bf);*/
    //}
    return true;
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

    m_group->removeChild(_node.get());

    // optimize the scene graph, remove redundant nodes and state etc.
    /*osgUtil::Optimizer optimizer;
    optimizer.optimize(m_group.get());*/

    osgViewer::View *view = m_viewer->getView(0);

    view->setSceneData( m_group );

    return true;
}

//bool OSGWidget::setSceneData(osg::ref_ptr<osg::Node> _sceneData)
//{
//    if (!_sceneData)
//    {
//        std::cout << "No data loaded" << std::endl;
//        return false;
//    }

//    m_models.push_back(_sceneData);


//    osgViewer::View *view = m_viewer->getView(0);

//    view->setSceneData( m_models.back().get() );


//    return true;
//}

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
        m_group->removeChild(m_models[i]);
        //m_models[i] = NULL; useless
    }

    m_models.clear();

    // remove all drawables
    // m_measurement_geode->removeDrawables(0,m_measurement_geode->getNumDrawables());
    for (unsigned int i=0; i<m_geodes.size(); i++)
    {
        m_geodes[i]->removeDrawables(0,m_geodes[i]->getNumDrawables());
        m_group->removeChild(m_geodes[i]);
        //m_models[i] = NULL; useless
    }
    m_geodes.clear();

    // reinit georef
    m_ref_lat_lon.setX(INVALID_VALUE);
    m_ref_lat_lon.setY(INVALID_VALUE);
    m_ref_depth = INVALID_VALUE;

    this->initializeGL();
}

void OSGWidget::initializeGL(){

    // Init properties
    osg::StateSet* state_set = m_group->getOrCreateStateSet();
    osg::Material* material = new osg::Material;
    material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );
    state_set->setAttributeAndModes( material, osg::StateAttribute::ON );
    state_set->setMode(GL_BLEND, osg::StateAttribute::ON);
    state_set->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    //stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );
}

void OSGWidget::paintGL()
{
    m_viewer->frame();
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
        _inter_point = hitr->getWorldIntersectPoint();

    }else{
        _inter_exists = false;
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
}

osgGA::EventQueue* OSGWidget::getEventQueue() const
{
    osgGA::EventQueue* event_queue = m_graphicsWindow->getEventQueue();

    if( event_queue )
        return event_queue;
    else
        throw std::runtime_error( "Unable to obtain valid event queue");
}

//osg::ref_ptr<osg::Geode> OSGWidget::getMeasurementGeode()
//{
//    return m_measurement_geode;
//}

//void OSGWidget::forceGeodeUpdate()
//{
//    m_group->removeChild(m_measurement_geode);
//    m_group->addChild(m_measurement_geode);
//}

void OSGWidget::getGeoOrigin(QPointF &_ref_lat_lon, double &_ref_depth)
{
    _ref_lat_lon = m_ref_lat_lon;
    _ref_depth = m_ref_depth;
}

// set initial values
void OSGWidget::setGeoOrigin(QPointF _latlon, double _depth)
{
    // Transform model
    osg::ref_ptr<osg::MatrixTransform> model_transform = new osg::MatrixTransform;
    m_ref_lat_lon = _latlon;
    m_ref_depth = _depth;
    m_ltp_proj.Reset(m_ref_lat_lon.x(), m_ref_lat_lon.y(),m_ref_depth);

    model_transform->setMatrix(osg::Matrix::identity()); //translate(0,0,0));
    osg::ref_ptr<osg::Node> node = new osg::Node();
    model_transform->addChild(node);
    addNodeToScene(model_transform);
}

void OSGWidget::addGeode(osg::ref_ptr<osg::Geode> _geode)
{
    m_group->addChild(_geode.get());
    m_geodes.push_back(_geode);
}

void OSGWidget::removeGeode(osg::ref_ptr<osg::Geode> _geode)
{
    // remove geode
    std::vector<osg::ref_ptr<osg::Geode>>::iterator position = std::find(m_geodes.begin(), m_geodes.end(), _geode);
    if (position != m_geodes.end()) // == myVector.end() means the element was not found
        m_geodes.erase(position);

    m_group->removeChild(_geode);
}

void OSGWidget::addGroup(osg::ref_ptr<osg::Group> _group)
{
    //m_groups.push_back(_group);
    m_group->addChild(_group.get());
}

void OSGWidget::removeGroup(osg::ref_ptr<osg::Group> _group)
{
    // remove group
    //    std::vector<osg::ref_ptr<osg::Group>>::iterator position = std::find(m_groups.begin(), m_groups.end(), _group);
    //    if (position != m_groups.end()) // == myVector.end() means the element was not found
    //        m_groups.erase(position);

    m_group->removeChild(_group);
}

// reset view to home
void OSGWidget::home()
{
    osgViewer::View *view = m_viewer->getView(0);
    view->home();
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

// convert x, y, z => lat, lon & depth
// if(m_ref_depth == INVALID_VALUE) do nothing
void OSGWidget::xyzToLatLonDepth(double _x, double _y, double _z, double &_lat, double &_lon, double &_depth)
{
    if(m_ref_depth == INVALID_VALUE)
        return;

    m_ltp_proj.Reverse(_x, _y, _z, _lat, _lon, _depth);
}


bool OSGWidget::generateGeoTiff(osg::ref_ptr<osg::Node> _node, QString _filename, double _pixel_size, OSGWidget::map_type _map_type)
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

    if ( _map_type == map_type::OrthoMap ) post_render_camera->addChild( geode );

    root->addChild(post_render_camera);

    // Create the viewer
    osgViewer::Viewer viewer;
    viewer.setThreadingModel( osgViewer::Viewer::SingleThreaded );
    viewer.setCamera( mrt_camera.get() );
    viewer.getCamera()->setProjectionMatrixAsOrtho2D(-width_meter/2,width_meter/2,-height_meter/2,height_meter/2);

    // put our model in the center of our viewer
    viewer.setCameraManipulator(new osgGA::TrackballManipulator());
    double cam_center_z= (x_max-x_min)/2;

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

    std::string screen_capture_filename = _filename.toStdString();

    if ( _map_type == map_type::OrthoMap )
    {
        SnapImage* final_draw_callback = new SnapImage(viewer.getCamera()->getGraphicsContext(),screen_capture_filename,m_ref_lat_lon, image_bounds,_pixel_size);
        mrt_camera->setFinalDrawCallback(final_draw_callback);
    }


    viewer.home();
    viewer.frame();
    if ( _map_type == map_type::DepthMap )
    {
        GDALAllRegister();
        CPLPushErrorHandler(CPLQuietErrorHandler);

        GDALDataset *geotiff_dataset_depth;
        GDALDriver *driver_geotiff_depth;

        int no_data =  -9999;
        std::string file_prof = screen_capture_filename+".tif";

        driver_geotiff_depth = GetGDALDriverManager()->GetDriverByName("GTiff");
        geotiff_dataset_depth = driver_geotiff_depth->Create(file_prof.c_str(),width_pixel,height_pixel,1,GDT_Float32,NULL);

        float buffer[width_pixel];

        QProgressDialog progress_dialog("Write depth map file...", "Abort depth map", 0, height_pixel, this);
        progress_dialog.setWindowModality(Qt::WindowModal);

        for(int i=0; i<height_pixel; i++) {
            for(int j=0; j<width_pixel; j++) {
                QApplication::processEvents();
                osg::Vec3d _inter_point;
                osgUtil::LineSegmentIntersector::Intersections intersections;
                progress_dialog.setValue(i);
                if (progress_dialog.wasCanceled())
                    return false;
                if (viewer.computeIntersections(viewer.getCamera(),osgUtil::Intersector::WINDOW,j,height_pixel-i,intersections))
                {

                    osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();

                    // we get the intersections in a osg::Vec3d
                    _inter_point = hitr->getWorldIntersectPoint();
                    float depth_point = _inter_point.z();
                    buffer[j] = depth_point;

                }else{
                    float depth_point = no_data;
                    buffer[j] = depth_point;
                }
            }
            // CPLErr GDALRasterBand::RasterIO( GDALRWFlag eRWFlag, int nXOff, int nYOff, int nXSize, int nYSize, void * pData, int nBufXSize, int nBufYSize, GDALDataType eBufType, int nPixelSpace, int nLineSpace )
            geotiff_dataset_depth->GetRasterBand(1)->RasterIO(GF_Write,0,i,width_pixel,1,buffer,width_pixel,1,GDT_Float32,0,0);
        }
        progress_dialog.setValue(height_pixel);
        geotiff_dataset_depth->GetRasterBand(1)->SetNoDataValue(no_data);

        // Setup output coordinate system that is UTM 11 WGS84.
        double geo_transform[6] = { image_bounds.xMin(), _pixel_size, 0, image_bounds.yMax(), 0, -_pixel_size };
        geotiff_dataset_depth->SetGeoTransform(geo_transform);
        char *geo_reference_depth = NULL;
        OGRSpatialReference o_SRS_depth;
        o_SRS_depth.SetTM(m_ref_lat_lon.x(),m_ref_lat_lon.y(),0.9996,0,0);
        o_SRS_depth.SetWellKnownGeogCS( "WGS84" );
        o_SRS_depth.exportToWkt( &geo_reference_depth );

        geotiff_dataset_depth->SetProjection(geo_reference_depth);
        CPLFree( geo_reference_depth );
        GDALClose(geotiff_dataset_depth) ;

        GDALDestroyDriverManager();
    }

    return true;


}

void OSGWidget::enableLight(bool _state)
{
    if ( _state )
    {
        m_viewer->getView(0)->getCamera()->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    }
    else
    {
        m_viewer->getView(0)->getCamera()->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    }
}

void OSGWidget::enableStereo(bool _state)
{
    //osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::VERTICAL_INTERLACE);
    osg::DisplaySettings::instance()->setStereo(_state);
}

void OSGWidget::slot_onTransparencyChange(int _transparency_value, osg::ref_ptr<osg::Node> _node)
{
    osg::StateSet* state_set = _node->getOrCreateStateSet();
    osg::StateAttribute* attr = state_set->getAttribute(osg::StateAttribute::MATERIAL);
    osg::Material* material = dynamic_cast<osg::Material*>(attr);

    //osg::Material* material = new osg::Material;
    /*float alpha = (float)_transparency_value/100;
    material->setAlpha( osg::Material::FRONT_AND_BACK, alpha );
    m_viewer->getView(0)->getCamera()->getOrCreateStateSet()->setMode( GL_BLEND,osg::StateAttribute::ON );
    m_viewer->getView(0)->getCamera()->getOrCreateStateSet()->setAttributeAndModes(material, osg::StateAttribute::ON);*/
   //osg::ref_ptr<osg::Material> material = new osg::Material;
    float alpha = (float)_transparency_value/100;
    //state_set->setMode( GL_BLEND, osg::StateAttribute::ON );
    //float alpha = (float)20/100;

    //material->setTransparency(osg::Material::FRONT, alpha );
    material->setAlpha(osg::Material::FRONT_AND_BACK, alpha );
    /*if ( m_fAlpha >= 1.0f ){
    //Entity is opaque so turn off state attribute
    stateSet->setAttributeAndModes( material,osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
    }else{*/
    //Entity has transparency
    //state_set->setAttributeAndModes( material, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
    //state_set->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::ONE_MINUS_SRC_ALPHA,osg::BlendFunc::SRC_ALPHA );
    //osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc(osg::BlendFunc::DST_COLOR, osg::BlendFunc::SRC_ALPHA_SATURATE );
    state_set->setAttributeAndModes(bf);


    state_set->setAttributeAndModes( material, osg::StateAttribute::OVERRIDE);

    //state_set->setMode( GL_BLEND, osg::StateAttribute::ON );

    //state_set->setTextureAttributeAndModes(0, ,osg::StateAttribute::ON );
    //state_set->setTextureMode(0,GL_BLEND, osg::StateAttribute::ON);
    //Transparency tr;
    //tr.setAlpha(alpha);
   // _node->accept(tr);
}
