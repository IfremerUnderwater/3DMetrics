#include "about_dialog.h"
#include "ui_about_dialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    connect(ui->close_button,SIGNAL(clicked(bool)),this,SLOT(close()));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
