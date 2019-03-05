#include "edit_measure_dialog.h"
#include "ui_edit_measure_dialog.h"
#include "edit_measure_item.h"

#include <QSet>

#include <QDebug>

#include <QFileInfo>
#include <QFile>
#include <QMessageBox>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "filedialog.h"

edit_measure_dialog::edit_measure_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::edit_measure_dialog)
{
    ui->setupUi(this);

    QObject::connect(ui->add_btn, SIGNAL(clicked(bool)), this, SLOT(slot_addItem()));
    QObject::connect(ui->load_pattern_btn, SIGNAL(clicked(bool)), this, SLOT(slot_loadPattern()));
    QObject::connect(ui->save_pattern_btn, SIGNAL(clicked(bool)), this, SLOT(slot_savePattern()));
    QObject::connect(ui->apply_btn, SIGNAL(clicked(bool)), this, SLOT(slot_apply()));
    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));
}

edit_measure_dialog::~edit_measure_dialog()
{
    delete ui;
}

edit_measure_item* edit_measure_dialog::addItem()
{
    QListWidgetItem* item;
    item = new QListWidgetItem(ui->listWidget);
    ui->listWidget->addItem(item);
    edit_measure_item *mit = new edit_measure_item(ui->listWidget,item);
    QSize sz(mit->width(),mit->height());
    item->setSizeHint(sz);
    ui->listWidget->setItemWidget(item, mit);
    QObject::connect(mit, SIGNAL(signal_remove(QListWidgetItem*)), this, SLOT(slot_remove(QListWidgetItem*)));

    return mit;
}

void edit_measure_dialog::slot_addItem()
{
    addItem();
}

void edit_measure_dialog::slot_remove(QListWidgetItem*item)
{
    ui->listWidget->setCurrentItem(item);
    int row = ui->listWidget->currentRow();
    ui->listWidget->removeItemWidget(item);
    QListWidgetItem *it = ui->listWidget->takeItem( row );
    delete it;
}

void edit_measure_dialog::slot_apply()
{
    MeasurePattern pattern = createPattern();

    // verify : all names different and not empty
    QSet<QString> fields;

    bool ok = true;
    int nb = pattern.getNbFields();
    for (int i=0; i<nb; i++)
    {
        if(pattern.fieldName(i).length() == 0)
        {
            QMessageBox::critical(this, tr("Measurement pattern"), tr("Empty field name not allowed"));
            ok = false;
            break;
        }
        if(fields.contains(pattern.fieldName(i)))
        {
            QMessageBox::critical(this, tr("Measurement pattern"), tr("Field names must be all different"));
            ok = false;
            break;
        }
        fields << pattern.fieldName(i);
    }

    if(ok)
    {
        emit signal_apply(pattern);
    }
}

void edit_measure_dialog::slot_loadPattern()
{
    QString fileName = getOpenFileName(this,tr("Select measurement pattern to open"), "", tr("Json files (*.json)"));
    if(fileName.length() > 0)
    {
        QFile f(fileName);
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray ba = f.readAll();
        f.close();

        //QJsonDocument doc = QJsonDocument::fromJson(ba);
        MeasurePattern pattern;
        bool res = pattern.loadFromJson(ba);
        if(!res)
        {
            QMessageBox::critical(this, tr("Error : measurement pattern"), tr("Error : invalid file"));
            return;
        }
        // clear list
        ui->listWidget->clear();

        // parse fields and add elements
        for(int i=0; i<pattern.getNbFields(); i++)
        {
            edit_measure_item *mit = addItem();
            mit->setValues(pattern.fieldName(i), pattern.fieldTypeName(i));
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Error : measurement pattern"), tr("Error : you didn't open measurement pattern"));
    }
}

void edit_measure_dialog::slot_savePattern()
{
    // save in file
    QString name = getSaveFileName(this, tr("Save measurement pattern"), "",
                                   "*.json");
    QFileInfo fileinfo(name);

    // check filename is not empty
    if(fileinfo.fileName().isEmpty()){
        QMessageBox::critical(this, tr("Error : save measurement pattern"), tr("Error : you didn't give a name to the pattern file"));
        return;
    }

    // add suffix if needed
    if (fileinfo.suffix() != "json"){
        name += ".json";
    }

    QFile file(name);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::critical(this, tr("Error : save measurement pattern file"), tr("Error : cannot open file for saving, check path writing rights"));
        return;
    }

    // build json object
    QJsonDocument json = createPattern().get();

    // write
    QString json_string = json.toJson();
    file.write(json_string.toUtf8());
    file.close();

    //qDebug() << json.toJson();
}

MeasurePattern edit_measure_dialog::createPattern()
{
    MeasurePattern pattern;

    for(int i=0; i<ui->listWidget->count(); i++)
    {
        QListWidgetItem *it = ui->listWidget->item(i);
        edit_measure_item *mit = (edit_measure_item*)ui->listWidget->itemWidget(it);
        pattern.addField(mit->fieldName(), mit->fieldType());
    }

    return pattern;
}

void edit_measure_dialog::setPattern(MeasurePattern pattern)
{
    // clear list
    ui->listWidget->clear();

    // pattern elements
    for(int i=0; i<pattern.getNbFields(); i++)
    {
        edit_measure_item *mit = addItem();
        mit->setValues(pattern.fieldName(i), pattern.fieldTypeName(i));
    }
}
