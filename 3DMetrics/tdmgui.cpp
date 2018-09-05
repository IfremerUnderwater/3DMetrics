#include "tdmgui.h"
#include "ui_tdmgui.h"

TDMGui::TDMGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TDMGui)
{
    ui->setupUi(this);
}

TDMGui::~TDMGui()
{
    delete ui;
}
