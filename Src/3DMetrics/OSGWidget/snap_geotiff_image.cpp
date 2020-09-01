#include "snap_geotiff_image.h"

#include <osg/RenderInfo>

#ifdef _WIN32
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

SnapGeotiffImage::SnapGeotiffImage(osg::GraphicsContext *_gc, const std::string &_filename, QPointF &_ref_lat_lon, osg::BoundingBox _box, double _pixel_size, QWidget *_parentWidget) :
    m_filename( _filename ),
    m_ref_lat_lon( _ref_lat_lon ),
    m_box( _box ),
    m_pixel_size( _pixel_size ),
    m_parentWidget(_parentWidget),
    m_status(true)
{
    m_image = new osg::Image;
    if (_gc->getTraits())
    {
        int width = _gc->getTraits()->width;
        int height = _gc->getTraits()->height;
        m_image->allocateImage(width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
    }
}

void SnapGeotiffImage::operator ()(osg::RenderInfo &renderInfo) const
{
    osg::Camera* camera = renderInfo.getCurrentCamera();

    osg::GraphicsContext* gc = camera->getGraphicsContext();
    if (gc->getTraits() && m_image.valid())
    {

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
        //GDALAllRegister();
        CPLPushErrorHandler(CPLQuietErrorHandler);
        GDALDataset *geotiff_dataset;
        GDALDriver *driver_geotiff;

        driver_geotiff = GetGDALDriverManager()->GetDriverByName("GTiff");
        geotiff_dataset = driver_geotiff->Create(tiff_name.c_str(),width,height,4,GDT_Byte,NULL);

        int size = height*width;
        unsigned char *buffer_R = new unsigned char[width];
        unsigned char *buffer_G = new unsigned char[width];
        unsigned char *buffer_B = new unsigned char[width];
        unsigned char *buffer_A = new unsigned char[width];

        QProgressDialog progress_dialog(QObject::tr("Write altitude ortho file..."), QObject::tr("Abort ortho map"), 0, height, m_parentWidget);
        progress_dialog.setWindowModality(Qt::WindowModal);
        progress_dialog.show();

        for(int i=0; i<height; i++)
        {
            progress_dialog.setValue(i);
            QApplication::processEvents();

            if (progress_dialog.wasCanceled())
            {
                // cleanup;
                GDALClose(geotiff_dataset) ;
                CPLPopErrorHandler();

                delete buffer_R;
                delete buffer_G;
                delete buffer_B;
                delete buffer_A;

                const_cast<SnapGeotiffImage*>(this)->m_status = false;

                return;
            }

            for(int j=0; j<(width); j++)
            {
                buffer_R[width-j-1] = m_image->data(size - ((width*i)+j) - 1)[0];
                buffer_G[width-j-1] = m_image->data(size - ((width*i)+j) - 1)[1];
                buffer_B[width-j-1] = m_image->data(size - ((width*i)+j) - 1)[2];
                buffer_A[width-j-1] = m_image->data(size - ((width*i)+j) - 1)[3];
            }
            // CPLErr GDALRasterBand::RasterIO( GDALRWFlag eRWFlag, int nXOff, int nYOff, int nXSize, int nYSize, void * pData, int nBufXSize, int nBufYSize, GDALDataType eBufType, int nPixelSpace, int nLineSpace )
            CPLErr res;
            res = geotiff_dataset->GetRasterBand(1)->RasterIO(GF_Write,0,i,width,1,buffer_R,width,1,GDT_Byte,0,0);
            res = geotiff_dataset->GetRasterBand(2)->RasterIO(GF_Write,0,i,width,1,buffer_G,width,1,GDT_Byte,0,0);
            res = geotiff_dataset->GetRasterBand(3)->RasterIO(GF_Write,0,i,width,1,buffer_B,width,1,GDT_Byte,0,0);
            res = geotiff_dataset->GetRasterBand(4)->RasterIO(GF_Write,0,i,width,1,buffer_A,width,1,GDT_Byte,0,0);
        }

        progress_dialog.setValue(height);

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

        CPLPopErrorHandler();

        delete buffer_R;
        delete buffer_G;
        delete buffer_B;
        delete buffer_A;

        const_cast<SnapGeotiffImage*>(this)->m_status = true;
        //GDALDestroyDriverManager();
    }
    else
    {
        const_cast<SnapGeotiffImage*>(this)->m_status = false;
    }

}


