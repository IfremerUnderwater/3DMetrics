#include <QApplication>
#include <QtWidgets>
#include "mainwindow.h"
#include "tdmgui.h"

int main(int argc, char *argv[])

{

    QApplication app(argc, argv);

    qRegisterMetaType<MeasInfo>();

    TDMGui tdm_gui;
    MainWindow main_window;

    main_window.show();
    tdm_gui.show();

    return app.exec();

}
