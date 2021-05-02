#include "ask_for_lod_dialog.h"
#include "ui_ask_for_lod_dialog.h"

AskForLODDialog::AskForLODDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AskForLODDialog)
{
    ui->setupUi(this);

    connect(ui->okButton,SIGNAL(clicked()),this,SLOT(ok()));
    connect(ui->cancelButton,SIGNAL(clicked()),this,SLOT(cancel()));
    connect(ui->saveCompLODCheck,SIGNAL(clicked()),this,SLOT(saveCompChecked()));

}

AskForLODDialog::~AskForLODDialog()
{
    delete ui;
}

// setters
void AskForLODDialog:: enableUseLOD(const bool _enable)
{
    ui->useLODCheck->setEnabled(_enable);
    if(!_enable)
    {
        setUseLOD(false);
    }
}

void AskForLODDialog::setUseLOD(const bool _use)
{
    ui->useLODCheck->setChecked(_use);
    m_useLOD = _use;
}

void AskForLODDialog::setBuildLOD(const bool _build)
{
    ui->buildLODCheck->setChecked(_build);
    m_setBuildLOD = _build;
}

void AskForLODDialog::setSaveCompLOD(const bool _save)
{
    ui->saveCompLODCheck->setChecked(_save);
    m_saveCompLOD = _save;
}

void AskForLODDialog::ok()
{
    m_useLOD = ui->useLODCheck->isChecked();
    m_setBuildLOD = ui->buildLODCheck->isChecked();
    m_saveCompLOD = ui->saveCompLODCheck->isChecked();

    accept();
}

void AskForLODDialog::cancel()
{
    reject();
}

void AskForLODDialog::saveCompChecked()
{
    if(ui->saveCompLODCheck->isChecked())
    {
        ui->buildLODCheck->setChecked(true);
    }
}
