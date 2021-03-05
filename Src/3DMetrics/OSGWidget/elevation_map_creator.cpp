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

#include <osgUtil/LineSegmentIntersector>

//#include <QDebug>

ElevationMapCreator::ElevationMapCreator(const std::string &_filename, QPointF &_ref_lat_lon, osg::BoundingBox _box, double _pixel_size, int _width_pixel, int _height_pixel) :
    m_filename( _filename ),
    m_ref_lat_lon( _ref_lat_lon ),
    m_box( _box ),
    m_pixel_size( _pixel_size ),
    m_width_pixel(_width_pixel),
    m_height_pixel(_height_pixel)
{

}

bool ElevationMapCreator::process(osgViewer::View &_view, QWidget *_parentWidget)
{
    CPLPushErrorHandler(CPLQuietErrorHandler);

    const int no_data =  -9999;
    std::string file_prof = m_filename+".tif";

    GDALDriver *driver_geotiff_alt = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset *geotiff_dataset_alt = driver_geotiff_alt->Create(file_prof.c_str(),m_width_pixel,m_height_pixel,1,GDT_Float32,NULL);

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
    double geo_transform[6] = { m_box.xMin(), m_pixel_size, 0, m_box.yMax(), 0, -m_pixel_size };
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
