#include <QApplication>
#include <QtWidgets>
#include "tdmgui.h"
// test (need to add includes and lib for X11 to Cmake)
//#include <X11/Xlib.h>

int main(int argc, char *argv[])

{

    //int res = XInitThreads();

    QApplication app(argc, argv);

    TDMGui tdm_gui;
    tdm_gui.show();

    return app.exec();

}
