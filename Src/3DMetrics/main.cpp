#include <QApplication>
#include <QtWidgets>
#include "tdmgui.h"

int main(int argc, char *argv[])

{

    QApplication app(argc, argv);

    TDMGui tdm_gui;
    tdm_gui.show();

    return app.exec();

}
