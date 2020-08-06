#include "grid_file_processor.h"
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Point>

#include <osgDB/WriteFile>
#include <osgDB/ReadFile>
#include <osg/LOD>
#include <osgUtil/Simplifier>
#include <osgUtil/Optimizer>
#include <osgUtil/SmoothingVisitor>

#ifdef _WIN32
#include "gdal_priv.h"
#include "cpl_conv.h"
#include "ogr_spatialref.h"
#else
#include "gdal/gdal_priv.h"
#include "gdal/cpl_conv.h"
#include "gdal/ogr_spatialref.h"
#endif

#include <GeographicLib/LocalCartesian.hpp>
#include <stdio.h>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QFileInfoList>

GridFileProcessor::GridFileProcessor()
{

}

GridFileProcessor::~GridFileProcessor()
{

}

///
/// \brief GridFileProcessor::loadFile
/// \param _scene_file
/// \param _mode
/// \param _local_lat_lon
/// \param _local_alt
/// \return
///
osg::ref_ptr<osg::Group> GridFileProcessor::loadFile(std::string _scene_file, LoadingMode _mode, QPointF &_local_lat_lon, double &_local_alt)
{
    osg::ref_ptr<osg::Group> group;

    GDALAllRegister();
    GDALDataset *dataset = (GDALDataset *) GDALOpen( _scene_file.c_str(), GA_ReadOnly );
    if(dataset != NULL)
    {
        char buffer[1024];
        double        adfGeoTransform[6];
        //                adfGeoTransform[0] /* top left x */
        //                adfGeoTransform[1] /* w-e pixel resolution */
        //                adfGeoTransform[2] /* 0 */
        //                adfGeoTransform[3] /* top left y */
        //                adfGeoTransform[4] /* 0 */
        //                adfGeoTransform[5] /* n-s pixel resolution (negative value) */
        sprintf(buffer, "Driver: %s/%s\n",
                dataset->GetDriver()->GetDescription(),
                dataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
        qDebug() << buffer;
        sprintf(buffer, "Size is %dx%dx%d\n",
                dataset->GetRasterXSize(), dataset->GetRasterYSize(),
                dataset->GetRasterCount() );
        qDebug() << buffer;
        if( dataset->GetProjectionRef()  != NULL )
        {
            sprintf(buffer, "Projection is `%s'\n", dataset->GetProjectionRef() );
            qDebug() << buffer;
        }
        if( dataset->GetGeoTransform( adfGeoTransform ) == CE_None )
        {
            sprintf(buffer, "Origin = (%.6f,%.6f)\n",
                    adfGeoTransform[0], adfGeoTransform[3] );
            qDebug() << buffer;
            sprintf(buffer, "Pixel Size = (%.6f,%.6f)\n",
                    adfGeoTransform[1], adfGeoTransform[5] );
            qDebug() << buffer;
        }

        GDALRasterBand  *poBand;
        int             nBlockXSize, nBlockYSize;
        int             bGotMin, bGotMax;
        double          adfMinMax[2];
        poBand = dataset->GetRasterBand( 1 );
        poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
        sprintf(buffer, "Block=%dx%d Type=%s, ColorInterp=%s\n",
                nBlockXSize, nBlockYSize,
                GDALGetDataTypeName(poBand->GetRasterDataType()),
                GDALGetColorInterpretationName(
                    poBand->GetColorInterpretation()) );
        qDebug() << buffer;

        adfMinMax[0] = poBand->GetMinimum( &bGotMin );
        adfMinMax[1] = poBand->GetMaximum( &bGotMax );
        if( ! (bGotMin && bGotMax) )
            GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
        sprintf(buffer, "Min=%.3fd, Max=%.3f\n", adfMinMax[0], adfMinMax[1] );
        qDebug() << buffer;

        if( poBand->GetOverviewCount() > 0 )
        {
            sprintf(buffer, "Band has %d overviews.\n", poBand->GetOverviewCount() );
            qDebug() << buffer;
        }

        if( poBand->GetColorTable() != NULL )
        {
            printf(buffer, "Band has a color table with %d entries.\n",
                   poBand->GetColorTable()->GetColorEntryCount() );
            qDebug() << buffer;
        }
        // read data
        float *pafScanline;
        float *pafScanline2;
        int   nXSize = poBand->GetXSize();
        int   nYSize = poBand->GetYSize();
        float noData = poBand->GetNoDataValue();

        // projection
        GeographicLib::LocalCartesian proj(adfGeoTransform[3],  adfGeoTransform[0]);
        _local_lat_lon.setX(adfGeoTransform[3]);
        _local_lat_lon.setY(adfGeoTransform[0]);
        _local_alt = 0;

        double deltaz = adfMinMax[1] - adfMinMax[0];

        group = new osg::Group;

        if(_mode == LoadingModePoint)
        {

            pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize);

            osg::ref_ptr<osg::Geode> geode = new osg::Geode;
            osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
            osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

            for(int y = 0; y < nYSize; y++)
            {

                // read line
                CPLErr err = poBand->RasterIO( GF_Read, 0, y, nXSize, 1,
                                               pafScanline, nXSize, 1, GDT_Float32,
                                               0, 0 );

                // build points

                // create point in geode
                // point
                for(int x=0; x<nXSize; x++)
                {
                    if( pafScanline[x] == noData) // check NAN
                        continue;

                    osg::Vec3f point;
                    double lon = adfGeoTransform[0] + adfGeoTransform[1]*x;
                    double lat = adfGeoTransform[3] + adfGeoTransform[5]*y;
                    double h = pafScanline[x];
                    double px, py, pz;
                    proj.Forward(lat, lon, h, px, py, pz);

                    point[0] = px;
                    point[1] = py;
                    point[2] = pz;

                    vertices->push_back(point);

                    // z color
                    double dh = (h - adfMinMax[0]) / deltaz;
                    float r = dh > 0.5 ? (dh - 0.5)*2: 0;
                    float g = dh > 0.5 ? (1.0 - dh) + 0.5 : (dh*2);
                    float b = 1.0 -dh;

                    // add a white color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
                    osg::Vec4 color(r, g, b,1.0f);
                    colors->push_back(color);
                }
            }

            // points
            osg::ref_ptr<osg::Geometry> shape_point_drawable = new osg::Geometry();

            // pass the created vertex array to the points geometry object.
            shape_point_drawable->setVertexArray(vertices);

            shape_point_drawable->setColorArray(colors, osg::Array::BIND_PER_VERTEX);

            // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
            // parameter passed to the DrawArrays constructor is the Primitive::Mode which
            // in this case is POINTS (which has the same value GL_POINTS), the second
            // parameter is the index position into the vertex array of the first point
            // to draw, and the third parameter is the number of points to draw.
            shape_point_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));

            // fixed size points
            shape_point_drawable->getOrCreateStateSet()->setAttribute(new osg::Point(1.f), osg::StateAttribute::ON);

            geode->addDrawable(shape_point_drawable);
            group->addChild(geode);

            // TOO SLOW!!!!!!!!
            //                osg::Geometry* geometry = new osg::Geometry();

            //                osg::ref_ptr<osgUtil::DelaunayTriangulator> dt = new
            //                osgUtil::DelaunayTriangulator(vertices);
            //                dt->triangulate(); // Generate the triangles
            //                geometry->setVertexArray(vertices);
            //                geometry->addPrimitiveSet(dt->getTriangles());
            //                geode->addDrawable(geometry);
            //                group->addChild(geode);

            CPLFree(pafScanline);
        }
        else if(_mode == LoadingModeTriangle || _mode == LoadingModeTriangleNormals)
        {
            // triangles

            pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize);
            pafScanline2 = (float *) CPLMalloc(sizeof(float)*nXSize);

            osg::ref_ptr<osg::Geode> geode = new osg::Geode;

            // read first line
            CPLErr err = poBand->RasterIO( GF_Read, 0, 0, nXSize, 1,
                                           pafScanline, nXSize, 1, GDT_Float32,
                                           0, 0 );

            for(int y = 1; y < nYSize; y++)
            {

                // read second line
                err = poBand->RasterIO( GF_Read, 0, y, nXSize, 1,
                                        pafScanline2, nXSize, 1, GDT_Float32,
                                        0, 0 );

                // create triangles in geode
                // AD
                // BC
                //  triangle 1 = ABC
                //  triangle 2 = ACD


                osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

                // for LoadingModeTriangleNormals
                osg::ref_ptr<osg::Vec3Array> normals;
                if(_mode == LoadingModeTriangleNormals)
                {
                    normals = new osg::Vec3Array;
                }

                osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

                // point
                for(int x=0; x<nXSize-1; x++)
                {
                    // check if 1 triangle is incomplète
                    if( pafScanline2[x] == noData) // check NAN
                        continue;
                    if( pafScanline2[x+1] == noData) // check NAN
                        continue;
                    if( pafScanline[x] == noData) // check NAN
                        continue;
                    if( pafScanline[x+1] == noData) // check NAN
                        continue;

                    // build triangle
                    osg::Vec3f pointA;
                    osg::Vec3f pointB;
                    osg::Vec3f pointC;
                    osg::Vec3f pointD;

                    // B
                    double lon = adfGeoTransform[0] + adfGeoTransform[1]*x;
                    double lat = adfGeoTransform[3] + adfGeoTransform[5]*(y+1);
                    double h = pafScanline2[x];
                    double px, py, pz;
                    proj.Forward(lat, lon, h, px, py, pz);
                    pointB[0] = px;
                    pointB[1] = py;
                    pointB[2] = pz;

                    // C
                    lon = adfGeoTransform[0] + adfGeoTransform[1]*(x+1);
                    lat = adfGeoTransform[3] + adfGeoTransform[5]*(y+1);
                    h = pafScanline2[x+1];
                    proj.Forward(lat, lon, h, px, py, pz);
                    pointC[0] = px;
                    pointC[1] = py;
                    pointC[2] = pz;

                    // A
                    lon = adfGeoTransform[0] + adfGeoTransform[1]*x;
                    lat = adfGeoTransform[3] + adfGeoTransform[5]*y;
                    h = pafScanline[x];
                    proj.Forward(lat, lon, h, px, py, pz);

                    pointA[0] = px;
                    pointA[1] = py;
                    pointA[2] = pz;


                    // D
                    lon = adfGeoTransform[0] + adfGeoTransform[1]*(x+1);
                    lat = adfGeoTransform[3] + adfGeoTransform[5]*y;
                    h = pafScanline[x+1];
                    proj.Forward(lat, lon, h, px, py, pz);

                    pointD[0] = px;
                    pointD[1] = py;
                    pointD[2] = pz;


                    // triangles
                    vertices->push_back(pointA);
                    vertices->push_back(pointB);
                    vertices->push_back(pointC);

                    if(_mode == LoadingModeTriangleNormals)
                    {
                        osg::Vec3f N1 = (pointB - pointA) ^ (pointC - pointB);
                        normals->push_back(N1);
                        normals->push_back(N1);
                        normals->push_back(N1);
                    }

                    vertices->push_back(pointA);
                    vertices->push_back(pointC);
                    vertices->push_back(pointD);


                    if(_mode == LoadingModeTriangleNormals)
                    {
                        osg::Vec3f N2 = (pointC - pointA) ^ (pointD - pointC);
                        normals->push_back(N2);
                        normals->push_back(N2);
                        normals->push_back(N2);
                    }

                }

                // triangles
                osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

                // pass the created vertex array to the points geometry object.
                geometry->setVertexArray(vertices);

                if(_mode == LoadingModeTriangleNormals)
                {
                    geometry->setNormalArray(normals, osg::Array::BIND_PER_VERTEX); //BIND_PER_PRIMITIVE_SET);
                }

                osg::Vec4 color(1.0,1.0,1.0,1.0);
                colors->push_back(color);
                geometry->setColorArray(colors, osg::Array::BIND_OVERALL); //BIND_PER_VERTEX);

                // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
                // parameter passed to the DrawArrays constructor is the Primitive::Mode which
                // in this case is POINTS (which has the same value GL_POINTS), the second
                // parameter is the index position into the vertex array of the first point
                // to draw, and the third parameter is the number of points to draw.
                geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices->size()));

                // fixed size points
                //shape_point_drawable->getOrCreateStateSet()->setAttribute(new osg::Point(1.f), osg::StateAttribute::ON);

                geode->addDrawable(geometry);

                // swap line ponters
                float * tmp = pafScanline;
                pafScanline = pafScanline2;
                pafScanline2 = tmp;
            }

            group->addChild(geode);

            CPLFree(pafScanline);
            CPLFree(pafScanline2);

        }
        else if(_mode == LoadingModeTrianglePoint)
        {
            // triangles + points

            pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize);
            pafScanline2 = (float *) CPLMalloc(sizeof(float)*nXSize);


            osg::ref_ptr<osg::Geode> geode = new osg::Geode;

            // read first line
            CPLErr err = poBand->RasterIO( GF_Read, 0, 0, nXSize, 1,
                                           pafScanline, nXSize, 1, GDT_Float32,
                                           0, 0 );

            for(int y = 1; y < nYSize; y++)
            {

                // read second line
                err = poBand->RasterIO( GF_Read, 0, y, nXSize, 1,
                                        pafScanline2, nXSize, 1, GDT_Float32,
                                        0, 0 );

                // create triangles in geode
                // AD
                // BC
                //  triangle 1 = ABC
                //  triangle 2 = ACD


                osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
                osg::ref_ptr<osg::Vec3Array> verticesPoint = new osg::Vec3Array;
                osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                osg::ref_ptr<osg::Vec4Array> colorst = new osg::Vec4Array;

                // point
                for(int x=0; x<nXSize-1; x++)
                {
                    // check if 1 triangle is incomplète
                    if( pafScanline2[x] == noData) // check NAN
                        continue;
                    if( pafScanline2[x+1] == noData) // check NAN
                        continue;
                    if( pafScanline[x] == noData) // check NAN
                        continue;
                    if( pafScanline[x+1] == noData) // check NAN
                        continue;

                    // build triangle
                    osg::Vec3f pointA;
                    osg::Vec3f pointB;
                    osg::Vec3f pointC;
                    osg::Vec3f pointD;
                    osg::Vec4 colorA;

                    // B
                    double lon = adfGeoTransform[0] + adfGeoTransform[1]*x;
                    double lat = adfGeoTransform[3] + adfGeoTransform[5]*(y+1);
                    double h = pafScanline2[x];
                    double px, py, pz;
                    proj.Forward(lat, lon, h, px, py, pz);
                    pointB[0] = px;
                    pointB[1] = py;
                    pointB[2] = pz;

                    // C
                    lon = adfGeoTransform[0] + adfGeoTransform[1]*(x+1);
                    lat = adfGeoTransform[3] + adfGeoTransform[5]*(y+1);
                    h = pafScanline2[x+1];
                    proj.Forward(lat, lon, h, px, py, pz);
                    pointC[0] = px;
                    pointC[1] = py;
                    pointC[2] = pz;

                    // A
                    lon = adfGeoTransform[0] + adfGeoTransform[1]*x;
                    lat = adfGeoTransform[3] + adfGeoTransform[5]*y;
                    h = pafScanline[x];
                    proj.Forward(lat, lon, h, px, py, pz);

                    pointA[0] = px;
                    pointA[1] = py;
                    pointA[2] = pz;

                    // z color
                    float dh = (h - adfMinMax[0]) / deltaz;
                    float r = dh > 0.5 ? (dh - 0.5)*2: 0;
                    float g = dh > 0.5 ? (1.0 - dh) + 0.5 : (dh*2);
                    float b = 1.0 -dh;

                    colorA = {r, g, b, 1.0f};

                    // D
                    lon = adfGeoTransform[0] + adfGeoTransform[1]*(x+1);
                    lat = adfGeoTransform[3] + adfGeoTransform[5]*y;
                    h = pafScanline[x+1];
                    proj.Forward(lat, lon, h, px, py, pz);

                    pointD[0] = px;
                    pointD[1] = py;
                    pointD[2] = pz;

                    // triangles
                    vertices->push_back(pointA);
                    vertices->push_back(pointB);
                    vertices->push_back(pointC);

                    vertices->push_back(pointA);
                    vertices->push_back(pointC);
                    vertices->push_back(pointD);

                    // Warning : Last row & last column ommitted
                    // points
                    verticesPoint->push_back(pointA);
                    colors->push_back(colorA);
                }

                // triangles
                osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

                // pass the created vertex array to the points geometry object.
                geometry->setVertexArray(vertices);

                //osg::Vec4 color(0.3,0.1,0.3,0.3);
                osg::Vec4 color(1.0,1.0,1.0,1.0);
                colorst->push_back(color);
                geometry->setColorArray(colorst, osg::Array::BIND_OVERALL); //BIND_PER_VERTEX);

                // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
                // parameter passed to the DrawArrays constructor is the Primitive::Mode which
                // in this case is POINTS (which has the same value GL_POINTS), the second
                // parameter is the index position into the vertex array of the first point
                // to draw, and the third parameter is the number of points to draw.
                geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices->size()));

                // fixed size points
                //shape_point_drawable->getOrCreateStateSet()->setAttribute(new osg::Point(1.f), osg::StateAttribute::ON);

                geode->addDrawable(geometry);

                // points
                osg::ref_ptr<osg::Geometry> geometryP = new osg::Geometry();

                // pass the created vertex array to the points geometry object.
                geometryP->setVertexArray(verticesPoint);
                geometryP->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
                geometryP->getOrCreateStateSet()->setAttribute(new osg::Point(1.f), osg::StateAttribute::ON);
                geometryP->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,verticesPoint->size()));
                geode->addDrawable(geometryP);


                // swap line poniters
                float * tmp = pafScanline;
                pafScanline = pafScanline2;
                pafScanline2 = tmp;
            }

            group->addChild(geode);

            CPLFree(pafScanline);
            CPLFree(pafScanline2);

        }

        GDALClose(dataset);

        GDALDestroyDriverManager();
    }

    return group;
}

// tile size in x and y
static const int TILESIZE = 256;

osg::ref_ptr<osg::Group> GridFileProcessor::loadFileAndBuildTiles(std::string _scene_file, QPointF &_local_lat_lon, double &_local_alt, bool _lod)
{
    osg::ref_ptr<osg::Group> group;

    GDALAllRegister();
    GDALDataset *dataset = (GDALDataset *) GDALOpen( _scene_file.c_str(), GA_ReadOnly );
    if(dataset != NULL)
    {
        char buffer[1024];
        double        adfGeoTransform[6];
        //                adfGeoTransform[0] /* top left x */
        //                adfGeoTransform[1] /* w-e pixel resolution */
        //                adfGeoTransform[2] /* 0 */
        //                adfGeoTransform[3] /* top left y */
        //                adfGeoTransform[4] /* 0 */
        //                adfGeoTransform[5] /* n-s pixel resolution (negative value) */
        sprintf(buffer, "Driver: %s/%s\n",
                dataset->GetDriver()->GetDescription(),
                dataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
        qDebug() << buffer;
        sprintf(buffer, "Size is %dx%dx%d\n",
                dataset->GetRasterXSize(), dataset->GetRasterYSize(),
                dataset->GetRasterCount() );
        qDebug() << buffer;
        if( dataset->GetProjectionRef()  != NULL )
        {
            sprintf(buffer, "Projection is `%s'\n", dataset->GetProjectionRef() );
            qDebug() << buffer;
        }
        if( dataset->GetGeoTransform( adfGeoTransform ) == CE_None )
        {
            sprintf(buffer, "Origin = (%.6f,%.6f)\n",
                    adfGeoTransform[0], adfGeoTransform[3] );
            qDebug() << buffer;
            sprintf(buffer, "Pixel Size = (%.6f,%.6f)\n",
                    adfGeoTransform[1], adfGeoTransform[5] );
            qDebug() << buffer;
        }

        GDALRasterBand  *poBand;
        int             nBlockXSize, nBlockYSize;
        int             bGotMin, bGotMax;
        double          adfMinMax[2];
        poBand = dataset->GetRasterBand( 1 );
        poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
        sprintf(buffer, "Block=%dx%d Type=%s, ColorInterp=%s\n",
                nBlockXSize, nBlockYSize,
                GDALGetDataTypeName(poBand->GetRasterDataType()),
                GDALGetColorInterpretationName(
                    poBand->GetColorInterpretation()) );
        qDebug() << buffer;

        adfMinMax[0] = poBand->GetMinimum( &bGotMin );
        adfMinMax[1] = poBand->GetMaximum( &bGotMax );
        if( ! (bGotMin && bGotMax) )
            GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
        sprintf(buffer, "Min=%.3fd, Max=%.3f\n", adfMinMax[0], adfMinMax[1] );
        qDebug() << buffer;

        if( poBand->GetOverviewCount() > 0 )
        {
            sprintf(buffer, "Band has %d overviews.\n", poBand->GetOverviewCount() );
            qDebug() << buffer;
        }

        if( poBand->GetColorTable() != NULL )
        {
            printf(buffer, "Band has a color table with %d entries.\n",
                   poBand->GetColorTable()->GetColorEntryCount() );
            qDebug() << buffer;
        }
        // read data
        float *pafScanline;
        float *pafScanline2;
        int   nXSize = poBand->GetXSize();
        int   nYSize = poBand->GetYSize();
        float noData = poBand->GetNoDataValue();

        // projection
        GeographicLib::LocalCartesian proj(adfGeoTransform[3],  adfGeoTransform[0]);
        _local_lat_lon.setX(adfGeoTransform[3]);
        _local_lat_lon.setY(adfGeoTransform[0]);
        _local_alt = 0;

        //double deltaz = adfMinMax[1] - adfMinMax[0];

        group = new osg::Group;

        // LoadingModeTriangle || _mode == LoadingModeTriangleNormals)
        {
            // triangles

            pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize);
            pafScanline2 = (float *) CPLMalloc(sizeof(float)*nXSize);


            const int ntilesX = nXSize / TILESIZE + (nXSize  % TILESIZE > 0 ? 1 : 0);
            const int ntilesY = nYSize / TILESIZE + (nYSize  % TILESIZE > 0 ? 1 : 0);

            osg::ref_ptr<osg::Geode> geode[ntilesX];
            for(int i=0; i<ntilesX; i++)
                geode[i] = new osg::Geode;

            // read first line
            CPLErr err = poBand->RasterIO( GF_Read, 0, 0, nXSize, 1,
                                           pafScanline, nXSize, 1, GDT_Float32,
                                           0, 0 );

            osg::ref_ptr<osg::Geometry> geometry[ntilesX];
            for(int i=0; i<ntilesX; i++)
            {
                geometry[i] = new osg::Geometry;
            }

            //            // for LoadingModeTriangleNormals
            //            osg::ref_ptr<osg::Vec3Array> normals[ntilesX];
            //            if(_normals)
            //            {
            //                for(int i=0; i<ntilesX; i++)
            //                {
            //                    normals[i] = new osg::Vec3Array;
            //                }
            //            }

            osg::ref_ptr<osg::Vec3Array> vertices[ntilesX];
            for(int i=0; i<ntilesX; i++)
            {
                vertices[i] = new osg::Vec3Array;
            }

            for(int y = 1; y < nYSize; y++)
            {
                // read second line
                err = poBand->RasterIO( GF_Read, 0, y, nXSize, 1,
                                        pafScanline2, nXSize, 1, GDT_Float32,
                                        0, 0 );

                // create triangles in geode
                // AD
                // BC
                //  triangle 1 = ABC
                //  triangle 2 = ACD

                // point
                for(int x=0; x<nXSize-1; x++)
                {
                    int index = x / TILESIZE;

                    // check if 1 triangle is incomplète
                    if( pafScanline2[x] == noData) // check NAN
                        continue;
                    if( pafScanline2[x+1] == noData) // check NAN
                        continue;
                    if( pafScanline[x] == noData) // check NAN
                        continue;
                    if( pafScanline[x+1] == noData) // check NAN
                        continue;

                    // build triangle
                    osg::Vec3f pointA;
                    osg::Vec3f pointB;
                    osg::Vec3f pointC;
                    osg::Vec3f pointD;

                    // B
                    double lon = adfGeoTransform[0] + adfGeoTransform[1]*x;
                    double lat = adfGeoTransform[3] + adfGeoTransform[5]*(y+1);
                    double h = pafScanline2[x];
                    double px, py, pz;
                    proj.Forward(lat, lon, h, px, py, pz);
                    pointB[0] = px;
                    pointB[1] = py;
                    pointB[2] = pz;

                    // C
                    lon = adfGeoTransform[0] + adfGeoTransform[1]*(x+1);
                    lat = adfGeoTransform[3] + adfGeoTransform[5]*(y+1);
                    h = pafScanline2[x+1];
                    proj.Forward(lat, lon, h, px, py, pz);
                    pointC[0] = px;
                    pointC[1] = py;
                    pointC[2] = pz;

                    // A
                    lon = adfGeoTransform[0] + adfGeoTransform[1]*x;
                    lat = adfGeoTransform[3] + adfGeoTransform[5]*y;
                    h = pafScanline[x];
                    proj.Forward(lat, lon, h, px, py, pz);

                    pointA[0] = px;
                    pointA[1] = py;
                    pointA[2] = pz;


                    // D
                    lon = adfGeoTransform[0] + adfGeoTransform[1]*(x+1);
                    lat = adfGeoTransform[3] + adfGeoTransform[5]*y;
                    h = pafScanline[x+1];
                    proj.Forward(lat, lon, h, px, py, pz);

                    pointD[0] = px;
                    pointD[1] = py;
                    pointD[2] = pz;


                    // triangles
                    vertices[index]->push_back(pointA);
                    vertices[index]->push_back(pointB);
                    vertices[index]->push_back(pointC);

                    //                    if(_normals)
                    //                    {
                    //                        osg::Vec3f N1 = (pointB - pointA) ^ (pointC - pointB);
                    //                        normals[index]->push_back(N1);
                    //                        normals[index]->push_back(N1);
                    //                        normals[index]->push_back(N1);
                    //                    }

                    vertices[index]->push_back(pointA);
                    vertices[index]->push_back(pointC);
                    vertices[index]->push_back(pointD);


                    //                    if(_normals)
                    //                    {
                    //                        osg::Vec3f N2 = (pointC - pointA) ^ (pointD - pointC);
                    //                        normals[index]->push_back(N2);
                    //                        normals[index]->push_back(N2);
                    //                        normals[index]->push_back(N2);
                    //                    }
                }

                //                for(int i=0; i<ntilesX; i++)
                //                {
                //                    if(vertices[i]->size() == 0)
                //                        continue;

                //                    // triangles
                //                    //osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

                //                    // pass the created vertex array to the points geometry object.
                //                    geometry->setVertexArray(vertices[i]);

                //                    if(_normals)
                //                    {
                //                        geometry->setNormalArray(normals[i], osg::Array::BIND_PER_VERTEX); //BIND_PER_PRIMITIVE_SET);
                //                    }

                //                    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                //                    osg::Vec4 color(1.0,1.0,1.0,1.0);
                //                    colors->push_back(color);
                //                    geometry->setColorArray(colors, osg::Array::BIND_OVERALL);

                //                    // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
                //                    // parameter passed to the DrawArrays constructor is the Primitive::Mode which
                //                    // in this case is POINTS (which has the same value GL_POINTS), the second
                //                    // parameter is the index position into the vertex array of the first point
                //                    // to draw, and the third parameter is the number of points to draw.
                //                    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices[i]->size()));

                //                    geode[i]->addDrawable(geometry);
                //                }
                // swap line ponters
                float * tmp = pafScanline;
                pafScanline = pafScanline2;
                pafScanline2 = tmp;

                // check next tile in y
                if((y % TILESIZE == 0) || (y == nYSize-1))
                {
                    for(int i=0; i<ntilesX; i++)
                    {
                        if(vertices[i]->size() == 0)
                            continue;

                        // pass the created vertex array to the points geometry object.
                        geometry[i]->setVertexArray(vertices[i]);

                        //                        if(_normals)
                        //                        {
                        //                            geometry[i]->setNormalArray(normals[i], osg::Array::BIND_PER_VERTEX); //BIND_PER_PRIMITIVE_SET);
                        //                        }

                        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                        osg::Vec4 color(1.0,1.0,1.0,1.0);
                        colors->push_back(color);
                        geometry[i]->setColorArray(colors, osg::Array::BIND_OVERALL);

                        // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
                        // parameter passed to the DrawArrays constructor is the Primitive::Mode which
                        // in this case is POINTS (which has the same value GL_POINTS), the second
                        // parameter is the index position into the vertex array of the first point
                        // to draw, and the third parameter is the number of points to draw.
                        geometry[i]->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices[i]->size()));

                        geode[i]->addDrawable(geometry[i]);

                        // TODO check and do not save empty files
                        unsigned int n = geode[i]->getNumDrawables();
                        if(n == 0)
                        {
                            qDebug() << "tile " << i << " " << ((y == nYSize-1) ? (y / TILESIZE) : (y / TILESIZE - 1))
                                     << " empty";
                            continue;
                        }

                        // recalculate normals with the smoothing visitor
                        osgUtil::SmoothingVisitor sv;
                        geode[i]->accept(sv);

                        // write
                        osg::ref_ptr<osg::Group> towrite = new osg::Group;
                        towrite->addChild(geode[i]);

                        std::string path = _scene_file;
                        // add tile number
                        char buffer[80];
                        sprintf(buffer, ".%03d_%03d",i, (y == nYSize-1) ? (y / TILESIZE) : (y / TILESIZE - 1));
                        path = path + buffer;
                        path = path + "-0.osgb";
                        osgDB::writeNodeFile(*towrite,
                                             path,
                                             new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib"));

                        qDebug() << "written " << geode[i]->getNumDrawables() << " " << path.c_str();

                        // LOD
                        if(_lod)
                        {
                            osgUtil::Simplifier simplifer;

                            simplifer.setSampleRatio(0.1f);
                            osg::ref_ptr<osg::Node> modelL1 = dynamic_cast<osg::Node *>(towrite->clone(osg::CopyOp::DEEP_COPY_ALL));
                            modelL1->accept(simplifer);

                            path = _scene_file;
                            path = path + buffer;
                            path = path + "-1.osgb";
                            osgDB::writeNodeFile(*modelL1,
                                                 path,
                                                 new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib"));


                            simplifer.setSampleRatio(0.2f);
                            osg::ref_ptr<osg::Node> modelL2 = dynamic_cast<osg::Node *>(modelL1->clone(osg::CopyOp::DEEP_COPY_ALL));
                            modelL2->accept(simplifer);

                            path = _scene_file;
                            path = path + buffer;
                            path = path + "-2.osgb";
                            osgDB::writeNodeFile(*modelL2,
                                                 path,
                                                 new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib"));


                            // coumpound LOD
                            bool _buildCompoundLOD = true;
                            if(_buildCompoundLOD)
                            {
                                osg::ref_ptr<osg::LOD> lod = new osg::LOD;
                                lod->addChild(towrite, 0,40.0f);
                                lod->addChild(modelL1.get(), 40.0f, 200.0f);
                                lod->addChild(modelL2, 200.0f, FLT_MAX);
                                path = _scene_file;
                                path = path + buffer;
                                path = path + ".osgb";
                                osgDB::writeNodeFile(*lod,
                                                     path,
                                                     new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib"));
                                group->addChild(lod);
                            }
                            else
                            {
                                group->addChild(modelL2);
                            }

                        }
                        else
                        {
                            // add to group
                            group->addChild(geode[i]);
                        }
                        // next
                        geode[i] = new osg::Geode;
                        geometry[i] = new osg::Geometry;
                        vertices[i] = new osg::Vec3Array;
                        //normals[i] = new osg::Vec3Array;
                    }
                }
            }

            CPLFree(pafScanline);
            CPLFree(pafScanline2);
        }

        GDALClose(dataset);

        GDALDestroyDriverManager();
    }

    return group;
}

osg::ref_ptr<osg::Group> GridFileProcessor::loadTiles(std::string _scene_file, std::string _subdir)
{
    osg::ref_ptr<osg::Group> group = new osg::Group;

    // build path to search
    std::string path;
    QFileInfo scene_info(QString::fromStdString(_scene_file));

    if(_subdir.size() > 0)
    {
        // use subdir
        path = scene_info.absoluteDir().absolutePath().toStdString();
        path = path + "/";
        path = path + _subdir;
    }
    else
    {
        path = scene_info.absoluteDir().absolutePath().toStdString();
    }
    QDir dir(path.c_str());
    QStringList pattern;
    pattern <<  (scene_info.fileName() + ".???_???.osgb");
    QFileInfoList files = dir.entryInfoList(pattern, QDir::Files );
    // load files
    for(int i=0; i<files.count(); i++)
    {
        qDebug() << files[i].fileName();
        osg::ref_ptr<osg::Node> node = osgDB::readRefNodeFile(files[i].absoluteFilePath().toStdString(), new osgDB::Options("noRotation"));
        group->addChild(node);

    }
    return group;
}

