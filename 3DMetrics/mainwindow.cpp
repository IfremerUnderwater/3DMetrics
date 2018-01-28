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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    m_model_file = QFileDialog::getOpenFileName(
                this,
                tr("Select one 3d Model to open"),
                "All files (*.*)");

    if(m_model_file.isNull())
        QMessageBox::information(this, tr("Error : 3d Model"), tr("Error : you didn't open a 3d model"));

    else
    {
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

    QString meas_name = m_measurement_form.getMeasName();
    QString meas_temp = m_measurement_form.getMeasTemp();
    QString meas_comment = m_measurement_form.getMeasComment();
    QString meas_category = m_measurement_form.getMeasCategory();

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

    // Add measurement to table
    addMeasToTable(meas_name, m_toolstate_to_qstring[m_tool_handler->getCurrentState()],
            meas_category, meas_temp, m_tool_handler->getTextFormattedResult(), meas_comment);
    ui->measurements_table->resizeColumnsToContents();

    // Set measurement name
    m_tool_handler->setCurrentMeasName(meas_name);

    // reset
    m_measurement_form.cleanMeasFormValues();
    goBackToIdle();
}


void MainWindow::addMeasToTable(QString _meas_name, QString _measur_type, QString _category, QString _temperature, QString _measur_result, QString _comments)
{

    QTableWidgetItem *checkbox = new QTableWidgetItem();
    checkbox->setCheckState(Qt::Checked);

    int row_count = ui->measurements_table->rowCount();
    ui->measurements_table->setRowCount(row_count+1);

    ui->measurements_table->setItem(row_count, 0, checkbox);
    ui->measurements_table->setItem(row_count, 1, new QTableWidgetItem(_meas_name));
    ui->measurements_table->setItem(row_count, 2, new QTableWidgetItem(QString(_measur_type))); // //////////////////
    ui->measurements_table->setItem(row_count, 3, new QTableWidgetItem(_category));
    ui->measurements_table->setItem(row_count, 4, new QTableWidgetItem(_temperature));
    ui->measurements_table->setItem(row_count, 5, new QTableWidgetItem(_measur_result));
    ui->measurements_table->setItem(row_count, 6, new QTableWidgetItem(_comments));

    m_qmap_measurement[_meas_name] = m_tool_handler->getMeasTypeAndIndex();
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
    m_delete_measurement_item = ui->measurements_table->itemAt(pos);

    m_delete_menu->popup(ui->measurements_table->viewport()->mapToGlobal(pos));
}


void MainWindow::sl_delete_measurement_action()
{

    int item_row = m_delete_measurement_item->row();
    QString item_name = ui->measurements_table->item(item_row,1)->text();

    m_tool_handler->removeMeasurementOfType(m_qmap_measurement[item_name].first, m_qmap_measurement[item_name].second);
    ui->measurements_table->removeRow(item_row);
}


void MainWindow::slot_openMeasureFile()
{
    //    m_measures_file = QFileDialog::getOpenFileName(
    //                this,
    //                tr("Select Measures file to open"),
    //                tr("Text files (*.txt *.csv)"));

    //    QFile file(m_measures_file);

    //    bool measur_file_is_null = m_measures_file.isNull();

    //    std::string new_measur_file_name = m_measures_file.toStdString();
    //    std::size_t position = new_measur_file_name.find_last_of(".\\");
    //    std::string file_name = new_measur_file_name.substr(position+1);

    //    if(measur_file_is_null)
    //        QMessageBox::information(this, tr("Error : 3d Model"), tr("Error : you didn't open a measurements file"));


}



void MainWindow::sl_saveMeasurFile()
{
    QString meas_filename = QFileDialog::getSaveFileName(
                this,
                tr("Save measurement file in JSON"),
                "*.json");

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

    QFile save_file(meas_filename);
    if(!save_file.open(QIODevice::WriteOnly)){
        QMessageBox::information(this, tr("Error : save measurement file"), tr("Error : cannot open file for saving, check path writing rights"));
        return;
    }

    // encode to Json
    QJsonObject root_obj;
    m_tool_handler->encodeToJSON(root_obj);

    QJsonDocument json_doc(root_obj);
    QString json_string = json_doc.toJson();
    save_file.write(json_string.toUtf8());
    save_file.close();

}
