#include "elevation_map_creator.h"

#if defined(_WIN32) || defined(__APPLE__)
#include "gdal_priv.h"
#include "cpl_conv.h"
#include "ogr_spatialref.h"
#else
#include "gdal/gdal_priv.h"
#include "gdal/cpl_conv.h"
#include "gdal/ogr_spatialref.h"
#endif

#include <QObject>
#include <QProgressDialog>
#include <QApplication>

#include <osg/MatrixTransform>
#include "box_visitor.h"

#include <osgUtil/LineSegmentIntersector>
#include <osgGA/TrackballManipulator>

bool ElevationMapCreator::processWrite(osgViewer::View &_view, osg::BoundingBox &box, const std::string &_filename, QWidget *_parentWidget)
{
    CPLPushErrorHandler(CPLQuietErrorHandler);

    const int no_data =  -9999;
    std::string filename = _filename+".tif";

    GDALDriver *driver_geotiff_alt = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset *geotiff_dataset_alt = driver_geotiff_alt->Create(filename.c_str(),m_width_pixel,m_height_pixel,1,GDT_Float32,NULL);

    float *buffer= new float[m_width_pixel];

    QProgressDialog progress_dialog(QObject::tr("Write altitude map file..."), QObject::tr("Abort altitude map"), 0, m_height_pixel, _parentWidget);
    progress_dialog.setWindowModality(Qt::WindowModal);

    const osg::Camera* camera = _view.getCamera();
    for(int i=0; i<m_height_pixel; i++)
    {
        progress_dialog.setValue(i);
        QApplication::processEvents();

        if (progress_dialog.wasCanceled())
        {
            // cleanup
            GDALClose(geotiff_dataset_alt) ;
            CPLPopErrorHandler();

            delete [] buffer;

            return false;
        }

        for(int j=0; j<m_width_pixel; j++) {

            osg::Vec3d _inter_point;
            osgUtil::LineSegmentIntersector::Intersections intersections;

            float x = j;
            float y = (m_height_pixel-i-1);
            if (_view.computeIntersections(camera,osgUtil::Intersector::WINDOW,x,y,intersections))
            {

                osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();

                // we get the intersections in a osg::Vec3d
                _inter_point = hitr->getWorldIntersectPoint();
                float alt_point = _inter_point.z();
                buffer[j] = alt_point;
                //qDebug()  << i << " " << j << " " << alt_point;

            }else{
                float alt_point = no_data;
                buffer[j] = alt_point;
                //qDebug() << i << " " << j << " Nodata";

            }
        }
        CPLErr res = geotiff_dataset_alt->GetRasterBand(1)->RasterIO(GF_Write,0,i,m_width_pixel,1,buffer,m_width_pixel,1,GDT_Float32,0,0);
    }

    delete [] buffer;

    progress_dialog.setValue(m_height_pixel);
    geotiff_dataset_alt->GetRasterBand(1)->SetNoDataValue(no_data);

    // Setup output coordinate system
    double geo_transform[6] = { box.xMin(), m_pixel_size, 0, box.yMax(), 0, -m_pixel_size };
    geotiff_dataset_alt->SetGeoTransform(geo_transform);
    char *geo_reference_alt = NULL;
    OGRSpatialReference o_SRS_alt;
    o_SRS_alt.SetTM(m_ref_lat_lon.x(),m_ref_lat_lon.y(),0.9996,0,0);
    o_SRS_alt.SetWellKnownGeogCS( "WGS84" );
    o_SRS_alt.exportToWkt( &geo_reference_alt );

    geotiff_dataset_alt->SetProjection(geo_reference_alt);
    CPLFree( geo_reference_alt );
    GDALClose(geotiff_dataset_alt) ;

    CPLPopErrorHandler();

    return true;
}

ElevationMapCreator::ElevationMapCreator(QPointF &_ref_lat_lon, double _pixel_size) :
    m_ref_lat_lon( _ref_lat_lon ),
    m_pixel_size( _pixel_size )
{

}

bool ElevationMapCreator::process(osg::ref_ptr<osg::Node> _node, const std::string &_filename, QWidget *_parentWidget)
{
    // get the translation in the  node
    osg::MatrixTransform *matrix_transform = dynamic_cast <osg::MatrixTransform*> (_node.get());
    osg::Vec3d translation = matrix_transform->getMatrix().getTrans();

    BoxVisitor boxVisitor;
    _node->accept(boxVisitor);

    osg::BoundingBox box = boxVisitor.getBoundingBox();

    // Create the edge of our picture
    double x_max = box.xMax();
    double x_min = box.xMin();
    double y_max = box.yMax();
    double y_min = box.yMin();
    m_width_pixel = ceil((x_max-x_min)/m_pixel_size);
    m_height_pixel = ceil((y_max-y_min)/m_pixel_size);
    double width_meter = m_pixel_size*m_width_pixel;
    double height_meter = m_pixel_size*m_height_pixel;
    double cam_center_x = (x_max+x_min)/2 +  translation.x();
    double cam_center_y = (y_max+y_min)/2 +  translation.y();

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 0;
    traits->y = 0;
    traits->width = m_width_pixel;
    traits->height = m_height_pixel;
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

    // Create the viewer
    osgViewer::Viewer viewer;
    viewer.setThreadingModel( osgViewer::Viewer::SingleThreaded );
    viewer.setRunFrameScheme( osgViewer::ViewerBase::ON_DEMAND );

    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setGraphicsContext(gc);
    camera->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    camera->setViewport( 0, 0, m_width_pixel, m_height_pixel );
    camera->setClearColor(osg::Vec4(0., 0., 0., 0.));

    viewer.setCamera( camera.get() );

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
    viewer.getCamera()->setProjectionMatrixAsOrtho2D(-width_meter/2,width_meter/2,-height_meter/2,height_meter/2);
    viewer.getCameraManipulator()->setHomePosition(eyes,center,normal);

    viewer.setSceneData( root.get() );
    viewer.realize();

    // setup the callback
    osg::BoundingBox image_bounds;
    image_bounds.xMin() = cam_center_x-width_meter/2;
    image_bounds.xMax() = cam_center_x+width_meter/2;
    image_bounds.yMin() = cam_center_y-height_meter/2;
    image_bounds.yMax() = cam_center_y+height_meter/2;

    viewer.home();
    viewer.frame();

    bool status = processWrite(viewer, image_bounds, _filename, _parentWidget);
    viewer.setSceneData(nullptr);

    return status;
}
