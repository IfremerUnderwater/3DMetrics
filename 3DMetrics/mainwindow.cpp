#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QVector>
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
    QObject::connect(&m_measurement_form, SIGNAL(sig_getMeasFormValues(QString,ToolState,QString,QString,QString,int,QString)), this, SLOT(slot_saveMeasFormValues(QString, ToolState,QString,QString,QString,int,QString)));
    QObject::connect(&m_measurement_form, SIGNAL(si_formSavingCanceled()), this, SLOT(sl_formSavingCanceled()));

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
            return;
        }
    }

    // Add measurement to table
    addMeasToTable(meas_name, m_toolstate_to_qstring[m_tool_handler->getCurrentState()],
            meas_category, meas_temp, m_tool_handler->getTextFormattedResult(), meas_comment);

    ui->measurements_table->resizeColumnsToContents();
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


void MainWindow::sl_formSavingCanceled()
{
    m_tool_handler->removeLastMeasurement();
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

    m_tool_handler->setCurrentToolState(IDLE_STATE);
    // /////////////////////////////////////////////////////////////////////////////////////////////////////////

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



void MainWindow::sl_show_hide_measurement(QTableWidgetItem *_item_clicked)
{  
    int item_row = _item_clicked->row();
    int item_column = _item_clicked->column();

    qDebug() << "Item " << item_row << ","<< item_column <<  " is clicked";

    if (item_column == 0)
    {
        QString item_name = ui->measurements_table->item(item_row,1)->text();
        qDebug() <<"Item measurement name : " << item_name;
        qDebug() << "item type : " << m_qmap_measurement[item_name].first;
        qDebug() << "item index : " << m_qmap_measurement[item_name].second;

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
    qDebug() << "Test rentre dans le menu context";

    m_delete_measurement_item = ui->measurements_table->itemAt(pos);
    int item_row = m_delete_measurement_item->row();
    QString item_name = ui->measurements_table->item(item_row,1)->text();

    qDebug() << "Item " << item_row;
    qDebug() <<"Item measurement name : " << item_name;
    qDebug() << "item type : " << m_qmap_measurement[item_name].first;
    qDebug() << "item index : " << m_qmap_measurement[item_name].second;

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

    //    else
    //    {
    //        if((file_name != "txt") && (file_name != "csv"))
    //            QMessageBox::information(this, tr("Error : save measurement file"), tr("Error : you didn't give a correct name to the measurement file"));

    //        else
    //        {
    //            if((!file.open(QIODevice::ReadOnly | QIODevice::Text)))
    //                QMessageBox::information(this, tr("Error : save measurement file"), tr("Error : you didn't give a correct name to the measurement file"));

    //            else
    //            {
    //                QTextStream in(&file);
    //                while (!in.atEnd())
    //                {
    //                    QString line = in.readLine();

    //                    if((line.isEmpty()) || line.startsWith("#"))
    //                        qDebug() << "error";

    //                    else
    //                    {

    //                        if(line == "begin_measurements_table")
    //                            m_position_measur_file = "begin_measur_table";

    //                        else if(line == "end_measurements_table")
    //                            m_position_measur_file = "end_measur_table";

    //                        else if(line == "begin_line_measurement")
    //                            m_position_measur_file = "begin_line_measur";

    //                        else if(line == "end_line_measurement")
    //                            m_position_measur_file = "end_line_measur";

    //                        else if(line == "begin_surface_measurement")
    //                            m_position_measur_file = "begin_surface_measur";

    //                        else if(line == "end_surface_measurement")
    //                            m_position_measur_file = "end_surface_measur";

    //                        else if(line == "begin_interest_point_measurement")
    //                            m_position_measur_file = "begin_interest_pt_measur";

    //                        else if(line == "end_interest_point_measurement")
    //                            m_position_measur_file = "end_interest_pt_measur";

    //                        else if(line == "begin_QMap_measurements")
    //                            m_position_measur_file = "begin_qmap_measur";

    //                        else if(line == "end_QMap_measurements")
    //                            m_position_measur_file = "end_qmap_measur";

    //                        else
    //                        {

    //                            if(m_position_measur_file == "begin_measur_table")
    //                            {
    //                                m_true_counter++;
    //                                qDebug() << m_true_counter;

    //                                QStringList fields_list = line.split(';');

    //                                add_measur_to_table(fields_list.at(0), fields_list.at(1),
    //                                                    fields_list.at(2), fields_list.at(3),
    //                                                    fields_list.at(4), fields_list.at(5));

    //                                qDebug() << fields_list;

    //                                ui->measurements_table->resizeColumnsToContents();
    //                            }

    //                            else if(m_position_measur_file == "begin_line_measur")
    //                            {

    //                            }

    //                            else if(m_position_measur_file == "begin_surface_measur")
    //                            {

    //                            }

    //                            else if(m_position_measur_file == "begin_interest_pt_measur")
    //                            {

    //                            }

    //                            else if(m_position_measur_file == "begin_qmap_measur")
    //                            {
    //                                QStringList fields_list = line.split(';');
    //                                bool ok;

    //                                QPair<QString,int> qpair_name_indice_measur;
    //                                qpair_name_indice_measur.first = fields_list.at(1);
    //                                qpair_name_indice_measur.second = fields_list.at(2).toInt(&ok, 10);

    //                                m_qmap_measurement[fields_list.at(0)] = qpair_name_indice_measur;
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
}



void MainWindow::sl_saveMeasurFile()
{
    //    QString measur_file_name = QFileDialog::getSaveFileName(
    //                this,
    //                tr("Save measurement file"),
    //                tr("Text files (*.txt *.csv)"));

    //    bool _measur_file_is_null = measur_file_name.isNull();

    //    std::string new_measur_file_name = measur_file_name.toStdString();

    //    std::size_t position = new_measur_file_name.find_last_of(".\\");
    //    std::string file_name = new_measur_file_name.substr(position+1);

    //    QString test = QString::fromStdString(file_name);
    //    qDebug() << "TEST : " << test;

    //    if(_measur_file_is_null)
    //        QMessageBox::information(this, tr("Error : save measurement file"), tr("Error : you didn't give a name to the measurement file"));

    //    else
    //    {
    //        if((file_name != "txt") && (file_name != "csv"))
    //            QMessageBox::information(this, tr("Error : save measurement file"), tr("Error : you didn't give a correct name to the measurement file"));

    //        else
    //        {
    //            int row_counter = ui->measurements_table->rowCount();
    //            int column_counter = ui->measurements_table->columnCount();

    //            if ((row_counter && column_counter) != 0)
    //            {
    //                std::ofstream fichier(new_measur_file_name, std::ios::out | std::ios::trunc);

    //                if(fichier)
    //                {
    //                    fichier << "begin_measurements_table" << std::endl << std::endl;
    //                    fichier << "#Name;Type;Category;Temperature;Result;Comments" << std::endl;


    //                    for(int i=0; i < row_counter; ++i)
    //                    {
    //                        for(int j=1; j < column_counter; ++j)
    //                        {
    //                            std::string text = ui->measurements_table->item(i,j)->text().toStdString();
    //                            fichier << text << ";";
    //                        }
    //                        fichier << std::endl;
    //                    }

    //                    fichier << std::endl;
    //                    fichier << "end_measurements_table" << std::endl << std::endl;


    //                    /*
    //                    QMap<int, osg::ref_ptr<osg::Vec3dArray> > measur_history_distance_qmap = ui->display_widget->getPointsCoordinates("Distance measurement");
    //                    QMap<int,int> measur_pts_distance_qmap = ui->display_widget->getMeasurPtsNumber("Distance measurement");
    //                    QMap<int,int> measur_lines_distance_qmap = ui->display_widget->getMeasurLinesNumber("Distance measurement");


    //                    QMap<int, osg::ref_ptr<osg::Vec3dArray> > measur_history_surface_qmap = ui->display_widget->getPointsCoordinates("Surface measurement");
    //                    QMap<int,int> measur_pts_surface_qmap = ui->display_widget->getMeasurPtsNumber("Surface measurement");
    //                    QMap<int,int> measur_lines_surface_qmap = ui->display_widget->getMeasurLinesNumber("Surface measurement");


    //                    QMap<int, osg::ref_ptr<osg::Vec3dArray> > measur_history_interest_pt_qmap = ui->display_widget->getPointsCoordinates("Interest Point measurement");
    //                    QMap<int,int> measur_pts_interest_qmap = ui->display_widget->getMeasurPtsNumber("Interest Point measurement");
    //                    */


    //                    fichier << "begin_line_measurement" << std::endl << std::endl;
    //                    fichier << "#distance measurement counter" << std::endl;
    //                    /*
    //                    fichier << QString::number(measur_history_distance_qmap.size()).toStdString() << std::endl << std::endl;

    //                    fichier << "#points counter for each measurement" << std::endl;
    //                    for(int i=0; i < measur_pts_distance_qmap.size(); ++i)
    //                    {
    //                        fichier << "Measurement " << QString::number(i+1).toStdString() << " : " << measur_pts_distance_qmap[i] << std::endl;
    //                    }
    //                    */
    //                    fichier << std::endl;


    //                    fichier << "#lines number for each measurement" << std::endl;
    //                    /*
    //                    for(int i=0; i < measur_lines_distance_qmap.size(); ++i)
    //                    {
    //                        fichier << "Measurement " << QString::number(i+1).toStdString() << " : " << measur_lines_distance_qmap[i] << std::endl;
    //                    }
    //                    */
    //                    fichier << std::endl;

    //                    fichier << "#coordinates points" << std::endl << std::endl;
    //                    /*
    //                    for(int i=0; i < measur_history_distance_qmap.size(); ++i)
    //                    {
    //                        for(int j=0; j < measur_history_distance_qmap[i]->size(); ++j)
    //                        {
    //                            fichier << QString::number(measur_history_distance_qmap[i]->at(j)[0]).toStdString() << ","
    //                                                                                 <<QString::number(measur_history_distance_qmap[i]->at(j)[1]).toStdString()
    //                                                                                 << ","
    //                                                                                 << QString::number(measur_history_distance_qmap[i]->at(j)[2]).toStdString()
    //                                                                                 << std::endl;
    //                        }
    //                        fichier << std::endl;
    //                    }
    //                    */


    //                    fichier << std::endl;
    //                    fichier << "end_line_measurement" << std::endl << std::endl;




    //                    fichier << "begin_surface_measurement" << std::endl << std::endl;
    //                    fichier << "#surface measurement counter" << std::endl;
    //                    /*
    //                    fichier << QString::number(measur_history_surface_qmap.size()).toStdString() << std::endl << std::endl;

    //                    fichier << "#points counter for each measurement" << std::endl;
    //                    for(int i=0; i < measur_pts_surface_qmap.size(); ++i)
    //                    {
    //                        fichier << "Measurement " << QString::number(i+1).toStdString() << " : " << measur_pts_surface_qmap[i] << std::endl;
    //                    }
    //                    */
    //                    fichier << std::endl;


    //                    fichier << "#lines number for each measurement" << std::endl;
    //                    /*
    //                    for(int i=0; i < measur_lines_surface_qmap.size(); ++i)
    //                    {
    //                        fichier << "Measurement " << QString::number(i+1).toStdString() << " : " << measur_lines_surface_qmap[i] << std::endl;
    //                    }
    //                    */
    //                    fichier << std::endl;


    //                    fichier << "#coordinates points" << std::endl << std::endl;
    //                    /*
    //                    for(int i=0; i < measur_history_surface_qmap.size(); ++i)
    //                    {
    //                        for(int j=0; j < measur_history_surface_qmap[i]->size(); ++j)
    //                        {
    //                            fichier << QString::number(measur_history_surface_qmap[i]->at(j)[0]).toStdString() << ","
    //                                                                                 <<QString::number(measur_history_surface_qmap[i]->at(j)[1]).toStdString()
    //                                                                                 << ","
    //                                                                                 << QString::number(measur_history_surface_qmap[i]->at(j)[2]).toStdString()
    //                                                                                 << std::endl;
    //                        }
    //                        fichier << std::endl;
    //                    }
    //                    */

    //                    fichier << std::endl;
    //                    fichier << "end_surface_measurement" << std::endl << std::endl;




    //                    fichier << "begin_interest_point_measurement" << std::endl << std::endl;
    //                    fichier << "#interest points measurement counter" << std::endl;
    //                    /*
    //                    fichier << QString::number(measur_history_interest_pt_qmap.size()).toStdString() << std::endl << std::endl;
    //                    */

    //                    fichier << "#coordinates points" << std::endl << std::endl;
    //                    /*
    //                    for(int i=0; i < measur_history_interest_pt_qmap.size(); ++i)
    //                    {
    //                        for(int j=0; j < measur_history_interest_pt_qmap[i]->size(); ++j)
    //                        {
    //                            fichier << QString::number(measur_history_interest_pt_qmap[i]->at(j)[0]).toStdString() << ","
    //                                                                                 <<QString::number(measur_history_interest_pt_qmap[i]->at(j)[1]).toStdString()
    //                                                                                 << ","
    //                                                                                 << QString::number(measur_history_interest_pt_qmap[i]->at(j)[2]).toStdString()
    //                                                                                 << std::endl;
    //                        }
    //                        fichier << std::endl;
    //                    }
    //                    */


    //                    fichier << std::endl;
    //                    fichier << "End_interest_point_measurement" << std::endl << std::endl;

    //                    fichier << "begin_QMap_measurements" << std::endl << std::endl << std::endl;
    //                    fichier << "#Measurement_name;Measurement_type;Measurement_index" << std::endl;

    //                    int measurement_qmap_size = m_qmap_measurement.count();

    //                    for(int i=0; i < measurement_qmap_size; ++i)
    //                    {
    //                        std::string measurement_name = ui->measurements_table->item(i,1)->text().toStdString();
    //                        std::string measurement_type = m_qmap_measurement[ui->measurements_table->item(i,1)->text()].first.toStdString();
    //                        int measurement_index = m_qmap_measurement[ui->measurements_table->item(i,1)->text()].second;

    //                        fichier << measurement_name << ";" << measurement_type << ";" << measurement_index << std::endl;
    //                    }

    //                    fichier << std::endl;
    //                    fichier << "end_QMap_measurements" << std::endl << std::endl;

    //                    fichier.close();
    //                }
    //            }
    //        }
    //    }
}
