#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QJsonObject>

#include <fstream>
#include <sstream>
#include <iostream>
#include "measurement_saving_dialog.h"

#include "filedialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menuBar->setNativeMenuBar(false);

    // Init tool handler
    m_tool_handler = new ToolHandler();
    m_tool_handler->setOsgWidget(ui->display_widget);

    ui->measurements_table->resizeColumnsToContents();

    m_delete_menu = new QMenu(ui->measurements_table);
    m_delete_measurement_action = new QAction("Delete measurement", this);
    m_delete_menu->addAction(m_delete_measurement_action);

    // connect ToolHandler to OSGWidget
    QObject::connect(ui->display_widget, SIGNAL(sig_onMousePress(Qt::MouseButton,int,int)), m_tool_handler, SLOT(slot_onMousePress(Qt::MouseButton,int,int)));

    // top menu bar
    QObject::connect(ui->quit_action, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->open_3d_model_action, SIGNAL(triggered()), this, SLOT(slot_open3dModel()));
    QObject::connect(ui->open_measure_file_action, SIGNAL(triggered()), this, SLOT(slot_openMeasureFile()));
    QObject::connect(ui->close_3d_model_action, SIGNAL(triggered()), this, SLOT(slot_close3dModel()));
    QObject::connect(ui->save_measure_file_action, SIGNAL(triggered()), this, SLOT(sl_saveMeasurFile()));

    // saving popup signals slots
    QObject::connect(m_tool_handler,SIGNAL(measurementEnded()),this,SLOT(slot_openMeasSavingPopup()));
    QObject::connect(&m_measurement_form, SIGNAL(si_measFormAccepted()), this, SLOT(slot_saveMeasFormValuesToTable()));
    QObject::connect(&m_measurement_form, SIGNAL(si_measFormCanceled()), this, SLOT(sl_formSavingCanceled()));

    QObject::connect(m_tool_handler,SIGNAL(sig_newMeasEndedWithInfo(MeasInfo)),this,SLOT(slot_addMeasToTable(MeasInfo)));

    // mainToolBar
    QObject::connect(ui->draw_segment_action, SIGNAL(triggered()), this, SLOT(sl_lineToolActivated()));
    QObject::connect(ui->draw_area_action, SIGNAL(triggered()), this, SLOT(sl_surfaceToolActivated()));
    QObject::connect(ui->draw_interest_point_action, SIGNAL(triggered()), this, SLOT(sl_interestPointToolActivated()));
    QObject::connect(ui->cancel_button, SIGNAL(triggered()), this, SLOT(sl_cancelMeasurement()));

    // hide/show measurement slot
    QObject::connect(ui->measurements_table, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(sl_show_hide_measurement(QTableWidgetItem*)));

    // delete measurement
    ui->measurements_table->setContextMenuPolicy(Qt::CustomContextMenu);
    //setContextMenuPolicy(Qt::ActionsContextMenu);
    QObject::connect(ui->measurements_table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(sl_contextMenuDeleteMeasurement(QPoint)));
    QObject::connect(m_delete_measurement_action, SIGNAL(triggered()), this, SLOT(sl_delete_measurement_action()));
    QObject::connect(ui->data_export_action, SIGNAL(triggered()), this, SLOT(slot_exportMeasToCSV()));

    // Correspondance table init
    m_toolstate_to_qstring[LINE_MEASUREMENT_STATE]="Line measurement";
    m_toolstate_to_qstring[SURFACE_MEASUREMENT_STATE]="Surface measurement";
    m_toolstate_to_qstring[INTEREST_POINT_STATE]="Interest point";

}

MainWindow::~MainWindow()
{
    delete m_delete_menu;
    delete m_delete_measurement_action;
    delete m_tool_handler;
    delete ui;
}


void MainWindow::slot_open3dModel()
{

    // Problem on Linux Ubuntu : to be replaced
//    m_model_file = QFileDialog::getOpenFileName(
//                this,
//                tr("Select one 3d Model to open"),
//                "/home",
//                "*.*");

    QString file = getOpenFileName(this,tr("Select one 3d Model to open"), "", tr("3D files (*.kml *.obj)"));

    if(file.length() == 0)
    {
        QMessageBox::information(this, tr("Error : 3d Model"), tr("Error : you didn't open a 3d model"));
    }
    else
    {
        m_model_file = file;
        ui->display_widget->setSceneFromFile(m_model_file.toStdString());
    }
}


void MainWindow::slot_close3dModel()
{

    int rows_nb = ui->measurements_table->rowCount();

    for(int i=0; i < rows_nb; ++i)
    {
        ui->measurements_table->removeRow((rows_nb-1)-i);
    }

    m_tool_handler->resetMeasData();
    ui->display_widget->clearSceneData();

}


void MainWindow::slot_openMeasSavingPopup()
{
    //m_last_meas_index = _measurement_index;
    m_measurement_form.setMeasFields(m_tool_handler->getTextFormattedResult(),m_toolstate_to_qstring[m_tool_handler->getCurrentState()]);
    m_measurement_form.show();
}



void MainWindow::slot_saveMeasFormValuesToTable()
{


    // check name
    QString meas_name = m_measurement_form.getMeasName();

    bool name_is_unique = true;
    bool name_is_empty = false;

    // Check measurement name is not empty and unique and ask again if necessary
    name_is_empty = meas_name.isEmpty();
    if( !ui->measurements_table->findItems(meas_name,Qt::MatchContains).isEmpty())
        name_is_unique = false;

    if(name_is_empty || !name_is_unique)
    {
        bool ok;
        meas_name = QInputDialog::getText(this, "Error name empty or not unique",
                                          "Please set another name.", QLineEdit::Normal,
                                          QString(), &ok);

        // Check again new name
        name_is_unique = true;
        name_is_empty = meas_name.isEmpty();
        if( !ui->measurements_table->findItems(meas_name,Qt::MatchContains).isEmpty())
            name_is_unique = false;

        if(!ok || name_is_empty || !name_is_unique)
        {
            m_tool_handler->removeLastMeasurement();
            if(name_is_empty)
                QMessageBox::critical(this, "Error : measurement name", "Empty measurement name so this measurement won't be saved.");
            if(!name_is_unique)
                QMessageBox::critical(this, "Error : measurement name", "Measurement name already exist so this measurement won't be saved.");
            m_measurement_form.cleanMeasFormValues();
            goBackToIdle();
            return;
        }
    }

    // create meas info
    MeasInfo meas_info;

    meas_info.name = meas_name;
    meas_info.type = m_tool_handler->getCurrentState();
    meas_info.index = m_tool_handler->getMeasTypeAndIndex().second;
    meas_info.category = m_measurement_form.getMeasCategory();
    meas_info.temperature = m_measurement_form.getMeasTemp();
    meas_info.formatted_result = m_tool_handler->getTextFormattedResult();
    meas_info.comments = m_measurement_form.getMeasComment();

    // Add measurement to table
    slot_addMeasToTable(meas_info);

    // Set measurement name
    m_tool_handler->setCurrentMeasInfo(meas_name, meas_info.comments, meas_info.temperature, meas_info.category);

    // reset
    m_measurement_form.cleanMeasFormValues();
    goBackToIdle();
}


void MainWindow::slot_addMeasToTable(MeasInfo _meas_info)
{

    QTableWidgetItem *checkbox = new QTableWidgetItem();
    checkbox->setCheckState(Qt::Checked);

    int row_count = ui->measurements_table->rowCount();
    ui->measurements_table->setRowCount(row_count+1);

    ui->measurements_table->setItem(row_count, 0, checkbox);
    ui->measurements_table->setItem(row_count, 1, new QTableWidgetItem(_meas_info.name));
    ui->measurements_table->setItem(row_count, 2, new QTableWidgetItem(m_toolstate_to_qstring[_meas_info.type])); // //////////////////
    ui->measurements_table->setItem(row_count, 3, new QTableWidgetItem(_meas_info.category));
    ui->measurements_table->setItem(row_count, 4, new QTableWidgetItem(_meas_info.temperature));
    ui->measurements_table->setItem(row_count, 5, new QTableWidgetItem(_meas_info.formatted_result));
    ui->measurements_table->setItem(row_count, 6, new QTableWidgetItem(_meas_info.comments));

    QPair<ToolState,int> type_idx_pair;
    type_idx_pair.first = _meas_info.type;
    type_idx_pair.second = _meas_info.index;
    m_qmap_measurement[_meas_info.name] = type_idx_pair;

    ui->measurements_table->resizeColumnsToContents();
}

void MainWindow::slot_exportMeasToCSV()
{

    QString filters("CSV files (*.csv);;All files (*.*)");
    QString defaultfilter("CSV files (*.csv)");
    QString filename = QFileDialog::getSaveFileName(0, "Save file", QCoreApplication::applicationDirPath(),
                                                    filters, &defaultfilter);

    QFileInfo file_name_info(filename);

    // check filename is not empty
    if(file_name_info.fileName().isEmpty()){
        QMessageBox::information(this, tr("Error : save measurement file"), tr("Error : you didn't give a name to the measurement file"));
        return;
    }

    QFile file(filename);

    QAbstractItemModel *model =  ui->measurements_table->model();
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream data(&file);
        QStringList strList;
        for (int i = 0; i < model->columnCount(); i++) {
            if (model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString().length() > 0)
                strList.append(model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
            else
                strList.append("");
        }
        data << strList.join(";") << "\n";
        for (int i = 0; i < model->rowCount(); i++) {
            strList.clear();
            for (int j = 0; j < model->columnCount(); j++) {

                if (model->data(model->index(i, j)).toString().length() > 0)
                    strList.append(model->data(model->index(i, j)).toString());
                else
                    strList.append("");
            }
            data << strList.join(";") + "\n";
        }
        file.close();
    }

}

void MainWindow::goBackToIdle()
{
    m_tool_handler->setCurrentToolState(IDLE_STATE);

    ui->draw_segment_action->setEnabled(true);
    ui->draw_area_action->setEnabled(true);
    ui->draw_interest_point_action->setEnabled(true);
    ui->cut_area_action->setEnabled(true);
    ui->draw_interest_point_action->setEnabled(true);
    ui->zoom_in_action->setEnabled(true);
    ui->zoom_out_action->setEnabled(true);
    ui->resize_action->setEnabled(true);
    ui->crop_action->setEnabled(true);
}


void MainWindow::sl_formSavingCanceled()
{
    m_tool_handler->removeLastMeasurement();
    goBackToIdle();
}


void MainWindow::sl_lineToolActivated()
{
    m_tool_handler->setCurrentToolState(LINE_MEASUREMENT_STATE);

    ui->draw_area_action->setEnabled(false);
    ui->draw_interest_point_action->setEnabled(false);
    ui->cut_area_action->setEnabled(false);
    ui->draw_interest_point_action->setEnabled(false);
    ui->zoom_in_action->setEnabled(false);
    ui->zoom_out_action->setEnabled(false);
    ui->resize_action->setEnabled(false);
    ui->crop_action->setEnabled(false);
}

void MainWindow::sl_surfaceToolActivated()
{
    m_tool_handler->setCurrentToolState(SURFACE_MEASUREMENT_STATE);

    ui->draw_segment_action->setEnabled(false);
    ui->draw_interest_point_action->setEnabled(false);
    ui->cut_area_action->setEnabled(false);
    ui->draw_interest_point_action->setEnabled(false);
    ui->zoom_in_action->setEnabled(false);
    ui->zoom_out_action->setEnabled(false);
    ui->resize_action->setEnabled(false);
    ui->crop_action->setEnabled(false);
}

void MainWindow::sl_interestPointToolActivated()
{
    m_tool_handler->setCurrentToolState(INTEREST_POINT_STATE);

    ui->draw_segment_action->setEnabled(false);
    ui->draw_area_action->setEnabled(false);
    ui->cut_area_action->setEnabled(false);
    ui->draw_interest_point_action->setEnabled(false);
    ui->zoom_in_action->setEnabled(false);
    ui->zoom_out_action->setEnabled(false);
    ui->resize_action->setEnabled(false);
    ui->crop_action->setEnabled(false);
}

void MainWindow::sl_cancelMeasurement()
{
    m_tool_handler->cancelMeasurement();
    goBackToIdle();
}



void MainWindow::sl_show_hide_measurement(QTableWidgetItem *_item_clicked)
{  
    int item_row = _item_clicked->row();
    int item_column = _item_clicked->column();

    if (item_column == 0)
    {
        QString item_name = ui->measurements_table->item(item_row,1)->text();

        // if item clicked is not checked
        if(_item_clicked->checkState() == Qt::Unchecked)
        {
            m_tool_handler->hideShowMeasurementOfType(m_qmap_measurement[item_name].first, m_qmap_measurement[item_name].second, false);
        }

        // if item clicked is checked
        else if(_item_clicked->checkState() == Qt::Checked)
        {
            m_tool_handler->hideShowMeasurementOfType(m_qmap_measurement[item_name].first, m_qmap_measurement[item_name].second, true);
        }
    }
}



void MainWindow::sl_contextMenuDeleteMeasurement(const QPoint & pos)
{
    //m_delete_measurement_item = ui->measurements_table->itemAt(pos);

    m_delete_menu->popup(ui->measurements_table->viewport()->mapToGlobal(pos));
}


void MainWindow::sl_delete_measurement_action()
{

    //int item_row = m_delete_measurement_item->row();
    QList<QTableWidgetItem*> selected_indexes = ui->measurements_table->selectedItems();
    //QString item_name = ui->measurements_table->item(item_row,1)->text();
    QList<QString> meas_names_to_be_removed;
    QVector<int> meas_rows_to_be_removed;

    foreach (QTableWidgetItem* selected_index, selected_indexes)
    {
        if(selected_index->column()==1)
        {

            meas_names_to_be_removed.push_back(selected_index->text());
            meas_rows_to_be_removed.push_front(selected_index->row());
        }
    }

    for (int i=0; i<meas_names_to_be_removed.size();i++)
    {
        m_tool_handler->removeMeasurementOfType(m_qmap_measurement[meas_names_to_be_removed[i]].first, m_qmap_measurement[meas_names_to_be_removed[i]].second);
        ui->measurements_table->removeRow(meas_rows_to_be_removed[i]);
    }
}


void MainWindow::slot_openMeasureFile()
{
    QString meas_filename = getOpenFileName(this,tr("Select measurements file to open"),"","*.json");
//            QFileDialog::getOpenFileName(
//                this,
//                tr("Select measurements file to open"),
//                "*.json");

    QFileInfo meas_file_info(meas_filename);

    // check filename is not empty
    if(meas_file_info.fileName().isEmpty()){
        QMessageBox::information(this, tr("Reading measurement file"), tr("Error : file opening canceled !"));
        return;
    }

    QFile meas_file(meas_filename);
    if(!meas_file.open(QIODevice::ReadOnly)){
        QMessageBox::information(this, tr("Reading measurement file"), tr("Error trying to open file !"));
        return;
    }

    QTextStream meas_file_text(&meas_file);
    QString json_string;
    json_string = meas_file_text.readAll();
    meas_file.close();
    QByteArray json_bytes = json_string.toLocal8Bit();

    QJsonDocument json_doc=QJsonDocument::fromJson(json_bytes);

    if(json_doc.isNull()){
        QMessageBox::information(this, tr("Reading measurement file"), tr("Failed to create Json"));
        return;
    }
    if(!json_doc.isObject()){
        QMessageBox::information(this, tr("Reading measurement file"), tr("Not containing Json object"));
        return;
    }

    QJsonObject json_obj=json_doc.object();

    if(json_obj.isEmpty()){
        QMessageBox::information(this, tr("Reading measurement file"), tr("Json object is empty"));
        return;
    }

    // decode Json
    m_tool_handler->decodeJSON(json_obj);

}



void MainWindow::sl_saveMeasurFile()
{
    QString meas_filename = getSaveFileName(this,
                                            tr("Save measurement file in JSON"),
                                            "",
                                            "*.json");
//            QFileDialog::getSaveFileName(
//                this,
//                tr("Save measurement file in JSON"),
//                "*.json");

    QFileInfo meas_file_info(meas_filename);

    // check filename is not empty
    if(meas_file_info.fileName().isEmpty()){
        QMessageBox::information(this, tr("Error : save measurement file"), tr("Error : you didn't give a name to the measurement file"));
        return;
    }

    // add suffix if needed
    if (meas_file_info.suffix() != "json"){
        meas_filename += ".json";
    }

    QFile meas_file(meas_filename);
    if(!meas_file.open(QIODevice::WriteOnly)){
        QMessageBox::information(this, tr("Error : save measurement file"), tr("Error : cannot open file for saving, check path writing rights"));
        return;
    }

    // encode to Json
    QJsonObject root_obj;
    m_tool_handler->encodeToJSON(root_obj);

    QJsonDocument json_doc(root_obj);
    QString json_string = json_doc.toJson();
    meas_file.write(json_string.toUtf8());
    meas_file.close();

}
