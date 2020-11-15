#include <QApplication>
#include <QtWidgets>
#include "tdmgui.h"

// GDAL
#if defined(_WIN32) || defined(__APPLE__)
#include "gdal_priv.h"
//#include "cpl_conv.h"
//#include "ogr_spatialref.h"
#else
#include "gdal/gdal_priv.h"
//#include "gdal/cpl_conv.h"
//#include "gdal/ogr_spatialref.h"
#endif

int main(int argc, char *argv[])

{
    QApplication app(argc, argv);

    // important for reading text format model files
    setlocale(LC_ALL, "C");

    // GDAL
    GDALAllRegister();

    TDMGui tdm_gui;
    tdm_gui.show();

    int res = app.exec();

    // GDAL
    GDALDestroyDriverManager();

    return res;

}
