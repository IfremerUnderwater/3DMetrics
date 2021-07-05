#include <QApplication>
#include <QtWidgets>
#include "tdmgui.h"
#include <QLocale>
#include "3DMetricsConfig.h"

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

    QLocale curLocale(QLocale("C"));
    QLocale::setDefault(curLocale);

    // GDAL
    GDALAllRegister();

    TDMGui tdm_gui;
    tdm_gui.setWindowTitle(QString("3D Metrics v%1.%2.%3").arg(TDM_VERSION_MAJOR).arg(TDM_VERSION_MINOR).arg(TDM_VERSION_PATCH));
    tdm_gui.show();

    int res = app.exec();

    // GDAL
    GDALDestroyDriverManager();

    return res;

}
