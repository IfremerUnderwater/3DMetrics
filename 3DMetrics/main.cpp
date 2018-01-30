#include <QApplication>
#include <QtWidgets>
#include "mainwindow.h"


int main(int argc, char *argv[])

{

    QApplication app(argc, argv);

    qRegisterMetaType<MeasInfo>();

    MainWindow main_window;

    main_window.show();

    return app.exec();

}
