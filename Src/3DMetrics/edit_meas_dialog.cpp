#include "edit_meas_dialog.h"
#include "ui_edit_meas_dialog.h"
#include "edit_meas_item.h"

#include <QSet>

#include <QDebug>

#include <QFileInfo>
#include <QFile>
#include <QMessageBox>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "file_dialog.h"

EditMeasDialog::EditMeasDialog(QWidget *_parent) :
    QDialog(_parent),
    ui(new Ui::EditMeasDialog)
{
    ui->setupUi(this);

    QObject::connect(ui->add_btn, SIGNAL(clicked(bool)), this, SLOT(slot_addItem()));
    QObject::connect(ui->load_pattern_btn, SIGNAL(clicked(bool)), this, SLOT(slot_loadPattern()));
    QObject::connect(ui->save_pattern_btn, SIGNAL(clicked(bool)), this, SLOT(slot_savePattern()));
    QObject::connect(ui->apply_btn, SIGNAL(clicked(bool)), this, SLOT(slot_apply()));
    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));
}

EditMeasDialog::~EditMeasDialog()
{
    delete ui;
}

EditMeasItem* EditMeasDialog::addItem()
{
    QListWidgetItem* item;
    item = new QListWidgetItem(ui->list_widget);
    ui->list_widget->addItem(item);
    EditMeasItem *edit_meas_item = new EditMeasItem(ui->list_widget,item);
    QSize size(edit_meas_item->width(),edit_meas_item->height());
    item->setSizeHint(size);
    ui->list_widget->setItemWidget(item, edit_meas_item);
    QObject::connect(edit_meas_item, SIGNAL(signal_remove(QListWidgetItem*)), this, SLOT(slot_remove(QListWidgetItem*)));

    return edit_meas_item;
}

void EditMeasDialog::slot_addItem()
{
    addItem();
}

void EditMeasDialog::slot_remove(QListWidgetItem*item)
{
    ui->list_widget->setCurrentItem(item);
    int row = ui->list_widget->currentRow();
    ui->list_widget->removeItemWidget(item);
    QListWidgetItem *it = ui->list_widget->takeItem( row );
    delete it;
}

void EditMeasDialog::slot_apply()
{
    MeasPattern pattern = createPattern();

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

void EditMeasDialog::slot_loadPattern()
{
    QString fileName = getOpenFileName(this,tr("Select measurement pattern to open"), "", tr("Json files (*.json)"));
    if(fileName.length() > 0)
    {
        QFile f(fileName);
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray ba = f.readAll();
        f.close();

        //QJsonDocument doc = QJsonDocument::fromJson(ba);
        MeasPattern pattern;
        bool res = pattern.loadFromJson(ba);
        if(!res)
        {
            QMessageBox::critical(this, tr("Error : measurement pattern"), tr("Error : invalid file"));
            return;
        }
        // clear list
        ui->list_widget->clear();

        // parse fields and add elements
        for(int i=0; i<pattern.getNbFields(); i++)
        {
            EditMeasItem *mit = addItem();
            mit->setValues(pattern.fieldName(i), pattern.fieldTypeName(i));
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Error : measurement pattern"), tr("Error : you didn't open measurement pattern"));
    }
}

void EditMeasDialog::slot_savePattern()
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

MeasPattern EditMeasDialog::createPattern()
{
    MeasPattern pattern;

    for(int i=0; i<ui->list_widget->count(); i++)
    {
        QListWidgetItem *it = ui->list_widget->item(i);
        EditMeasItem *mit = (EditMeasItem*)ui->list_widget->itemWidget(it);
        pattern.addField(mit->fieldName(), mit->fieldType());
    }

    return pattern;
}

void EditMeasDialog::setPattern(MeasPattern _pattern)
{
    // clear list
    ui->list_widget->clear();

    // pattern elements
    for(int i=0; i<_pattern.getNbFields(); i++)
    {
        EditMeasItem *mit = addItem();
        mit->setValues(_pattern.fieldName(i), _pattern.fieldTypeName(i));
    }
}
