#include <QApplication>
#include <QtWidgets>
#include "tdmgui.h"

int main(int argc, char *argv[])

{
    QApplication app(argc, argv);

    // important for reading text format model files
    setlocale(LC_ALL, "C");

    TDMGui tdm_gui;
    tdm_gui.show();

    return app.exec();

}
