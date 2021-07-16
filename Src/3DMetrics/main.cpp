#include <QApplication>
#include <QtWidgets>
#include <QLocale>
#include "CustomWidgets/framelesswindow.h"

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

    // Main window container
    FramelessWindow fless_windows;
    fless_windows.show();

    int res = app.exec();

    // GDAL
    GDALDestroyDriverManager();

    return res;

}
