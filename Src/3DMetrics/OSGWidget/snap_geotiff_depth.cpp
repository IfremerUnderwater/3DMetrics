#include "snap_geotiff_depth.h"

#include <osg/MatrixTransform>
#include <osgGA/TrackballManipulator>

#include "box_visitor.h"
//#include "node_user_data.h"

#if defined(_WIN32) || defined(__APPLE__)
#include "gdal_priv.h"
#include "cpl_conv.h"
#include "ogr_spatialref.h"
#else
#include "gdal/gdal_priv.h"
#include "gdal/cpl_conv.h"
#include "gdal/ogr_spatialref.h"
#endif

#include <QProgressDialog>
#include <QApplication>
#include <QMessageBox>

SnapGeotiffDepth::SnapGeotiffDepth(osgViewer::Viewer* pView)
{
    pViewer = pView;

    // Install custom renderer
    osg::ref_ptr<CustomRenderer> customRenderer = new CustomRenderer(pViewer->getCamera());
    pViewer->getCamera()->setRenderer(customRenderer.get());

    ////Important
    pViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);

    // Do cull and draw to render the scene correctly
    customRenderer->setCullOnly(false);
}

bool SnapGeotiffDepth::Capture(osg::ref_ptr<osg::Node> _node, std::string fileName, const double _pixel_size, double _refLatitude, double _refLongitude, QWidget *_parentWidget)
{   

    osg::GLExtensions *ext = new osg::GLExtensions(0);
    GLint maxTextureSize = ext->maxTextureSize;

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
    double width_pixel = ceil((x_max-x_min)/_pixel_size);
    double height_pixel = ceil((y_max-y_min)/_pixel_size);
    double width_meter = _pixel_size*width_pixel;
    double height_meter = _pixel_size*height_pixel;
    double cam_center_x = (x_max+x_min)/2 +  translation.x();
    double cam_center_y = (y_max+y_min)/2 +  translation.y();

    if( width_pixel > maxTextureSize ||  height_pixel > maxTextureSize)
    {
        // too big to process
        QString message = QObject::tr("Size Error:\n");
        if(width_pixel > maxTextureSize )
            message += QObject::tr("Width ") + QString::number(width_pixel) + " > " + QString::number(maxTextureSize) + "\n";
        if(height_pixel > maxTextureSize )
            message += QObject::tr("Height ") + QString::number(height_pixel) + " > " + QString::number(maxTextureSize) + "\n";
        message +=  QObject::tr("Pixel Size = ") + QString::number(_pixel_size) + " too small";
        QMessageBox::critical(_parentWidget, QObject::tr("Error : GeoTIFF Creation"), message);

        return false;
    }

    const int PCT = 100;
    int lastpct = 0;
    QProgressDialog progress_dialog(QObject::tr("Write altitude ortho file..."), QObject::tr("Abort ortho map"), 0, PCT, _parentWidget);
    progress_dialog.setWindowModality(Qt::WindowModal);
    progress_dialog.show();
    QApplication::processEvents();

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 0;
    traits->y = 0;
    traits->width = width_pixel;
    traits->height = height_pixel;
    traits->pbuffer = true;
    traits->alpha =  8;
    traits->depth = 24; //32; does not work in Windows
    traits->sharedContext = 0;
    traits->doubleBuffer = false;
    traits->samples = 0;
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
    camera->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera->setViewport( 0, 0, (int)width_pixel, (int)height_pixel );
    camera->setClearColor(osg::Vec4(0., 0., 0., 0.));
    viewer.setCamera( camera.get() );

    osg::ref_ptr<osg::Image> zImage = new osg::Image;
    zImage->allocateImage((int)width_pixel, (int)height_pixel, 1, GL_DEPTH_COMPONENT , GL_FLOAT);
    camera->attach(osg::Camera::DEPTH_BUFFER, zImage.get()); /* */

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

    viewer.frame();

    SnapGeotiffDepth *snap = new SnapGeotiffDepth(&viewer);

    if (snap->pViewer)
    {
        // Add the WindowCaptureCallback now that we have full resolution
        GLenum buffer = GL_FRONT;

        WindowCaptureCallback *winCaptureCbk = new WindowCaptureCallback(buffer, fileName);
        snap->pViewer->getCamera()->setFinalDrawCallback(winCaptureCbk);
        snap->pViewer->renderingTraversals();

        float zmin = box.zMin();
        float zmax = box.zMax();

        float delta = zmax - zmin;

        // GDAL
        {
            int width = width_pixel;
            int height = height_pixel;
            GDALDriver *driver_geotiff_alt = GetGDALDriverManager()->GetDriverByName("GTiff");
            GDALDataset *geotiff_dataset_alt = driver_geotiff_alt->Create((fileName + ".tif").c_str(),width,height,1,GDT_Float32,NULL);

            zImage->readPixels(0,0,width,height, GL_DEPTH_COMPONENT, GL_FLOAT);
            float *buffer= new float[width];
            const float no_data = -9999.0f;

            for(int i=0; i<height; i++)
            {
                int pct = i * PCT / height;
                if(pct != lastpct)
                {
                    progress_dialog.setValue(pct);
                    QApplication::processEvents();
                    lastpct = pct;
                }

                float*data = (float*)zImage->data();

                for(int j=0; j<width; j++)
                {
                    float val = data[(height-i-1)*width + j];
                    if(val == 1.0f)
                        buffer[j] = no_data;
                    else
                        buffer[j] = (1.0f - val) * delta + zmin;
                }
                CPLErr res = geotiff_dataset_alt->GetRasterBand(1)->RasterIO(GF_Write,0,i,width,1,buffer,width,1,GDT_Float32,0,0);
            }

            progress_dialog.setValue(PCT);
            QApplication::processEvents();

            delete [] buffer;

            geotiff_dataset_alt->GetRasterBand(1)->SetNoDataValue(no_data);

            // Setup output coordinate system
            double geo_transform[6] = { box.xMin(), _pixel_size, 0, box.yMax(), 0, -_pixel_size };
            geotiff_dataset_alt->SetGeoTransform(geo_transform);
            char *geo_reference_alt = NULL;
            OGRSpatialReference o_SRS_alt;
            o_SRS_alt.SetTM(_refLatitude,_refLongitude,0.9996,0,0);
            o_SRS_alt.SetWellKnownGeogCS( "WGS84" );
            o_SRS_alt.exportToWkt( &geo_reference_alt );

            geotiff_dataset_alt->SetProjection(geo_reference_alt);
            CPLFree( geo_reference_alt );

            GDALClose(geotiff_dataset_alt) ;
        }

        snap->pViewer->getCamera()->removeFinalDrawCallback(winCaptureCbk);
        snap->pViewer->getCamera()->setFinalDrawCallback(NULL);
    }
    viewer.done();
    viewer.setSceneData(nullptr);
    delete snap;

    return true;
}

SnapGeotiffDepth::~SnapGeotiffDepth()
{
}
