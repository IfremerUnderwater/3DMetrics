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
    ui(new Ui::MainWindow),
    m_last_meas_index(-1),
    m_true_counter(0),
    m_tool_state(IDLE_STATE),
    m_measurement_index(0)
{
    ui->setupUi(this);

    ui->measurements_table->resizeColumnsToContents();
    m_measurement_form.setWindowTitle("Measurement form");

    m_state_names[IDLE_STATE]="No selected tool";
    m_state_names[LINE_MEASUREMENT_STATE]="Distance measurement";
    m_state_names[SURFACE_MEASUREMENT_STATE]="Surface measurement";
    m_state_names[INTEREST_POINT_STATE]="Interest point measurement";
    m_state_names[CUT_AREA_TOOL_STATE]="Cut area tool";
    m_state_names[ZOOM_IN_TOOL_STATE]="Zoom in tool";
    m_state_names[ZOOM_OUT_TOOL_STATE]="Zoom out tool";
    m_state_names[FULL_SCREEN_TOOL_STATE]="Full screen tool";
    m_state_names[CROP_TOOL_STATE]="Crop tool";

    m_qmap_convert_state_names["No selected tool"]=IDLE_STATE;
    m_qmap_convert_state_names["Distance measurement"]=LINE_MEASUREMENT_STATE;
    m_qmap_convert_state_names["Surface measurement"]=SURFACE_MEASUREMENT_STATE;
    m_qmap_convert_state_names["Interest point measurement"]=INTEREST_POINT_STATE;
    m_qmap_convert_state_names["Cut area tool"]=CUT_AREA_TOOL_STATE;
    m_qmap_convert_state_names["Zoom in tool"]=ZOOM_IN_TOOL_STATE;
    m_qmap_convert_state_names["Zoom out tool"]=ZOOM_OUT_TOOL_STATE;
    m_qmap_convert_state_names["Full screen tool"]=FULL_SCREEN_TOOL_STATE;
    m_qmap_convert_state_names["Crop tool"]=CROP_TOOL_STATE;  

    m_delete_menu = new QMenu(ui->measurements_table);
    m_delete_measurement_action = new QAction("Delete measurement", this);
    m_delete_menu->addAction(m_delete_measurement_action);


    // menuBar
    QObject::connect(ui->quit_action, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->open_3d_model_action, SIGNAL(triggered()), this, SLOT(slot_open3dModel()));
    QObject::connect(ui->open_measure_file_action, SIGNAL(triggered()), this, SLOT(slot_openMeasureFile()));
    QObject::connect(ui->close_3d_model_action, SIGNAL(triggered()), this, SLOT(slot_close3dModel()));
    QObject::connect(ui->save_measure_file_action, SIGNAL(triggered()), this, SLOT(sl_saveMeasurFile()));

    QObject::connect(ui->display_widget, SIGNAL(sig_showMeasurementSavingPopup(double,QString,int)), this, SLOT(slot_openDistanceSurfaceMeasFromPopup(double,QString,int)));

    QObject::connect(ui->display_widget, SIGNAL(si_showInterestPointMeasurementSavingPopup(QString,QString,int)), this, SLOT(sl_openInterestPointMeasFromPopup(QString,QString,int)));
    QObject::connect(&m_measurement_form, SIGNAL(sig_getMeasFormValues(QString,QString,QString,QString,QString,int,QString)), this, SLOT(slot_saveMeasFormValues(QString, QString,QString,QString,QString,int,QString)));

    QObject::connect(&m_measurement_form, SIGNAL(si_distanceMeasurementFormCanceled()), this, SLOT(sl_distanceMeasurementFormCanceled()));
    QObject::connect(&m_measurement_form, SIGNAL(si_surfaceMeasurementFormCanceled()), this, SLOT(sl_surfaceMeasurementFormCanceled()));
    QObject::connect(&m_measurement_form, SIGNAL(si_interestPointMeasurementFormCanceled()), this, SLOT(sl_interestPointMeasurementFormCanceled()));

    QObject::connect(this, SIGNAL(si_errorNameDistanceMeasurementFormCanceled()), this, SLOT(sl_distanceMeasurementFormCanceled()));
    QObject::connect(this, SIGNAL(si_errorNameSurfaceMeasurementFormCanceled()), this, SLOT(sl_surfaceMeasurementFormCanceled()));
    QObject::connect(this, SIGNAL(si_errorNameInterestPointMeasurementFormCanceled()), this, SLOT(sl_interestPointMeasurementFormCanceled()));


    // mainToolBar
    QObject::connect(ui->draw_segment_action, SIGNAL(triggered()), ui->display_widget, SLOT(slot_setInLineMeasurementState()));
    QObject::connect(ui->draw_area_action, SIGNAL(triggered()), ui->display_widget, SLOT(slot_setInSurfaceMeasurementState()));
    QObject::connect(ui->draw_interest_point_action, SIGNAL(triggered()), ui->display_widget, SLOT(slot_setInInterestPointState()));
    QObject::connect(ui->cut_area_action, SIGNAL(triggered()), ui->display_widget, SLOT(slot_setInCutAreaState()));
    QObject::connect(ui->zoom_in_action, SIGNAL(triggered()), ui->display_widget, SLOT(slot_setInZoomInState()));
    QObject::connect(ui->zoom_out_action, SIGNAL(triggered()), ui->display_widget, SLOT(slot_setInZoomOutState()));
    QObject::connect(ui->resize_action, SIGNAL(triggered()), ui->display_widget, SLOT(slot_setInFullScreenState()));
    QObject::connect(ui->crop_action, SIGNAL(triggered()), ui->display_widget, SLOT(slot_setInCropState()));


    QObject::connect(ui->draw_segment_action, SIGNAL(triggered()), this, SLOT(sl_lineToolActivated()));
    QObject::connect(ui->draw_area_action, SIGNAL(triggered()), this, SLOT(sl_surfaceToolActivated()));
    QObject::connect(ui->draw_interest_point_action, SIGNAL(triggered()), this, SLOT(sl_interestPointToolActivated()));

    QObject::connect(ui->cancel_button, SIGNAL(triggered()), this, SLOT(sl_deactivateTool()));
    QObject::connect(ui->cancel_button, SIGNAL(triggered()), ui->display_widget, SLOT(sl_resetMeasur()));
    QObject::connect(ui->display_widget, SIGNAL(si_endMeasur()), this, SLOT(sl_deactivateTool()));
    QObject::connect(ui->display_widget, SIGNAL(si_returnIdleState()), this, SLOT(sl_returnIdleState()));
    QObject::connect(ui->display_widget, SIGNAL(si_returnIdleState()), ui->display_widget, SLOT(slot_setInIdleState()));

    // hide/show measurement slot
    QObject::connect(ui->measurements_table, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(sl_show_hide_measurement(QTableWidgetItem*)));
    // delete measurement
    ui->measurements_table->setContextMenuPolicy(Qt::CustomContextMenu);
    //setContextMenuPolicy(Qt::ActionsContextMenu);
    QObject::connect(ui->measurements_table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(sl_contextMenuDeleteMeasurement(QPoint)));
    QObject::connect(m_delete_measurement_action, SIGNAL(triggered()), this, SLOT(sl_delete_measurement_action()));

}

MainWindow::~MainWindow()
{
    delete m_delete_menu;
    delete m_delete_measurement_action;
    delete ui;
}


void MainWindow::slot_open3dModel()
{

    m_model_file = QFileDialog::getOpenFileName(
                this,
                tr("Select one 3d Model to open"),
                "All files (*.*)");


    bool model_file = m_model_file.isNull();

    if(model_file)
        QMessageBox::information(this, tr("Error : 3d Model"), tr("Error : you didn't open a 3d model"));

    else
    {
        ui->display_widget->setSceneFromFile(m_model_file.toStdString());
    }
}


void MainWindow::slot_openMeasureFile()
{
    m_measures_file = QFileDialog::getOpenFileName(
                this,
                tr("Select Measures file to open"),
                tr("Text files (*.txt *.csv)"));

    QFile file(m_measures_file);

    bool measur_file_is_null = m_measures_file.isNull();

    std::string new_measur_file_name = m_measures_file.toStdString();
    std::size_t position = new_measur_file_name.find_last_of(".\\");
    std::string file_name = new_measur_file_name.substr(position+1);

    if(measur_file_is_null)
        QMessageBox::information(this, tr("Error : 3d Model"), tr("Error : you didn't open a measurements file"));

    else
    {
        if((file_name != "txt") && (file_name != "csv"))
            QMessageBox::information(this, tr("Error : save measurement file"), tr("Error : you didn't give a correct name to the measurement file"));

        else
        {
            if((!file.open(QIODevice::ReadOnly | QIODevice::Text)))
                QMessageBox::information(this, tr("Error : save measurement file"), tr("Error : you didn't give a correct name to the measurement file"));

            else
            {
                QTextStream in(&file);
                while (!in.atEnd())
                {
                    QString line = in.readLine();

                    if((line.isEmpty()) || line.startsWith("#"))
                        qDebug() << "error";

                    else
                    {

                        if(line == "begin_measurements_table")
                            m_position_measur_file = "begin_measur_table";

                        else if(line == "end_measurements_table")
                            m_position_measur_file = "end_measur_table";

                        else if(line == "begin_line_measurement")
                            m_position_measur_file = "begin_line_measur";

                        else if(line == "end_line_measurement")
                            m_position_measur_file = "end_line_measur";

                        else if(line == "begin_surface_measurement")
                            m_position_measur_file = "begin_surface_measur";

                        else if(line == "end_surface_measurement")
                            m_position_measur_file = "end_surface_measur";

                        else if(line == "begin_interest_point_measurement")
                            m_position_measur_file = "begin_interest_pt_measur";

                        else if(line == "end_interest_point_measurement")
                            m_position_measur_file = "end_interest_pt_measur";

                        else if(line == "begin_QMap_measurements")
                            m_position_measur_file = "begin_qmap_measur";

                        else if(line == "end_QMap_measurements")
                            m_position_measur_file = "end_qmap_measur";

                        else
                        {

                            if(m_position_measur_file == "begin_measur_table")
                            {
                                m_true_counter++;
                                qDebug() << m_true_counter;

                                QStringList fields_list = line.split(';');

                                add_measur_to_table(fields_list.at(0), fields_list.at(1),
                                                    fields_list.at(2), fields_list.at(3),
                                                    fields_list.at(4), fields_list.at(5));

                                qDebug() << fields_list;

                                ui->measurements_table->resizeColumnsToContents();
                            }

                            else if(m_position_measur_file == "begin_line_measur")
                            {

                            }

                            else if(m_position_measur_file == "begin_surface_measur")
                            {

                            }

                            else if(m_position_measur_file == "begin_interest_pt_measur")
                            {

                            }

                            else if(m_position_measur_file == "begin_qmap_measur")
                            {
                                QStringList fields_list = line.split(';');
                                bool ok;

                                QPair<QString,int> qpair_name_indice_measur;
                                qpair_name_indice_measur.first = fields_list.at(1);
                                qpair_name_indice_measur.second = fields_list.at(2).toInt(&ok, 10);

                                m_qmap_measurement[fields_list.at(0)] = qpair_name_indice_measur;
                            }
                        }
                    }
                }
            }
        }
    }
}



void MainWindow::slot_close3dModel()
{

    int row_counter = ui->measurements_table->rowCount();

    for(int i=0; i < row_counter; ++i)
    {
        ui->measurements_table->removeRow((row_counter-1)-i);
    }

    ui->display_widget->clearSceneData();

    m_true_counter=0;
}



void MainWindow::sl_saveMeasurFile()
{
    QString measur_file_name = QFileDialog::getSaveFileName(
                this,
                tr("Save measurement file"),
                tr("Text files (*.txt *.csv)"));

    bool _measur_file_is_null = measur_file_name.isNull();

    std::string new_measur_file_name = measur_file_name.toStdString();

    std::size_t position = new_measur_file_name.find_last_of(".\\");
    std::string file_name = new_measur_file_name.substr(position+1);

    QString test = QString::fromStdString(file_name);
    qDebug() << "TEST : " << test;

    if(_measur_file_is_null)
        QMessageBox::information(this, tr("Error : save measurement file"), tr("Error : you didn't give a name to the measurement file"));

    else
    {
        if((file_name != "txt") && (file_name != "csv"))
            QMessageBox::information(this, tr("Error : save measurement file"), tr("Error : you didn't give a correct name to the measurement file"));

        else
        {
            int row_counter = ui->measurements_table->rowCount();
            int column_counter = ui->measurements_table->columnCount();

            if ((row_counter && column_counter) != 0)
            {
                std::ofstream fichier(new_measur_file_name, std::ios::out | std::ios::trunc);

                if(fichier)
                {
                    fichier << "begin_measurements_table" << std::endl << std::endl;
                    fichier << "#Name;Type;Category;Temperature;Result;Comments" << std::endl;


                    for(int i=0; i < row_counter; ++i)
                    {
                        for(int j=1; j < column_counter; ++j)
                        {
                            std::string text = ui->measurements_table->item(i,j)->text().toStdString();
                            fichier << text << ";";
                        }
                        fichier << std::endl;
                    }

                    fichier << std::endl;
                    fichier << "end_measurements_table" << std::endl << std::endl;


                    /*
                    QMap<int, osg::ref_ptr<osg::Vec3dArray> > measur_history_distance_qmap = ui->display_widget->getPointsCoordinates("Distance measurement");
                    QMap<int,int> measur_pts_distance_qmap = ui->display_widget->getMeasurPtsNumber("Distance measurement");
                    QMap<int,int> measur_lines_distance_qmap = ui->display_widget->getMeasurLinesNumber("Distance measurement");


                    QMap<int, osg::ref_ptr<osg::Vec3dArray> > measur_history_surface_qmap = ui->display_widget->getPointsCoordinates("Surface measurement");
                    QMap<int,int> measur_pts_surface_qmap = ui->display_widget->getMeasurPtsNumber("Surface measurement");
                    QMap<int,int> measur_lines_surface_qmap = ui->display_widget->getMeasurLinesNumber("Surface measurement");


                    QMap<int, osg::ref_ptr<osg::Vec3dArray> > measur_history_interest_pt_qmap = ui->display_widget->getPointsCoordinates("Interest Point measurement");
                    QMap<int,int> measur_pts_interest_qmap = ui->display_widget->getMeasurPtsNumber("Interest Point measurement");
                    */


                    fichier << "begin_line_measurement" << std::endl << std::endl;
                    fichier << "#distance measurement counter" << std::endl;
                    /*
                    fichier << QString::number(measur_history_distance_qmap.size()).toStdString() << std::endl << std::endl;

                    fichier << "#points counter for each measurement" << std::endl;
                    for(int i=0; i < measur_pts_distance_qmap.size(); ++i)
                    {
                        fichier << "Measurement " << QString::number(i+1).toStdString() << " : " << measur_pts_distance_qmap[i] << std::endl;
                    }
                    */
                    fichier << std::endl;


                    fichier << "#lines number for each measurement" << std::endl;
                    /*
                    for(int i=0; i < measur_lines_distance_qmap.size(); ++i)
                    {
                        fichier << "Measurement " << QString::number(i+1).toStdString() << " : " << measur_lines_distance_qmap[i] << std::endl;
                    }
                    */
                    fichier << std::endl;

                    fichier << "#coordinates points" << std::endl << std::endl;
                    /*
                    for(int i=0; i < measur_history_distance_qmap.size(); ++i)
                    {
                        for(int j=0; j < measur_history_distance_qmap[i]->size(); ++j)
                        {
                            fichier << QString::number(measur_history_distance_qmap[i]->at(j)[0]).toStdString() << ","
                                                                                 <<QString::number(measur_history_distance_qmap[i]->at(j)[1]).toStdString()
                                                                                 << ","
                                                                                 << QString::number(measur_history_distance_qmap[i]->at(j)[2]).toStdString()
                                                                                 << std::endl;
                        }
                        fichier << std::endl;
                    }
                    */


                    fichier << std::endl;
                    fichier << "end_line_measurement" << std::endl << std::endl;




                    fichier << "begin_surface_measurement" << std::endl << std::endl;
                    fichier << "#surface measurement counter" << std::endl;
                    /*
                    fichier << QString::number(measur_history_surface_qmap.size()).toStdString() << std::endl << std::endl;

                    fichier << "#points counter for each measurement" << std::endl;
                    for(int i=0; i < measur_pts_surface_qmap.size(); ++i)
                    {
                        fichier << "Measurement " << QString::number(i+1).toStdString() << " : " << measur_pts_surface_qmap[i] << std::endl;
                    }
                    */
                    fichier << std::endl;


                    fichier << "#lines number for each measurement" << std::endl;
                    /*
                    for(int i=0; i < measur_lines_surface_qmap.size(); ++i)
                    {
                        fichier << "Measurement " << QString::number(i+1).toStdString() << " : " << measur_lines_surface_qmap[i] << std::endl;
                    }
                    */
                    fichier << std::endl;


                    fichier << "#coordinates points" << std::endl << std::endl;
                    /*
                    for(int i=0; i < measur_history_surface_qmap.size(); ++i)
                    {
                        for(int j=0; j < measur_history_surface_qmap[i]->size(); ++j)
                        {
                            fichier << QString::number(measur_history_surface_qmap[i]->at(j)[0]).toStdString() << ","
                                                                                 <<QString::number(measur_history_surface_qmap[i]->at(j)[1]).toStdString()
                                                                                 << ","
                                                                                 << QString::number(measur_history_surface_qmap[i]->at(j)[2]).toStdString()
                                                                                 << std::endl;
                        }
                        fichier << std::endl;
                    }
                    */

                    fichier << std::endl;
                    fichier << "end_surface_measurement" << std::endl << std::endl;




                    fichier << "begin_interest_point_measurement" << std::endl << std::endl;
                    fichier << "#interest points measurement counter" << std::endl;
                    /*
                    fichier << QString::number(measur_history_interest_pt_qmap.size()).toStdString() << std::endl << std::endl;
                    */

                    fichier << "#coordinates points" << std::endl << std::endl;
                    /*
                    for(int i=0; i < measur_history_interest_pt_qmap.size(); ++i)
                    {
                        for(int j=0; j < measur_history_interest_pt_qmap[i]->size(); ++j)
                        {
                            fichier << QString::number(measur_history_interest_pt_qmap[i]->at(j)[0]).toStdString() << ","
                                                                                 <<QString::number(measur_history_interest_pt_qmap[i]->at(j)[1]).toStdString()
                                                                                 << ","
                                                                                 << QString::number(measur_history_interest_pt_qmap[i]->at(j)[2]).toStdString()
                                                                                 << std::endl;
                        }
                        fichier << std::endl;
                    }
                    */


                    fichier << std::endl;
                    fichier << "End_interest_point_measurement" << std::endl << std::endl;

                    fichier << "begin_QMap_measurements" << std::endl << std::endl << std::endl;
                    fichier << "#Measurement_name;Measurement_type;Measurement_index" << std::endl;

                    int measurement_qmap_size = m_qmap_measurement.count();

                    for(int i=0; i < measurement_qmap_size; ++i)
                    {
                        std::string measurement_name = ui->measurements_table->item(i,1)->text().toStdString();
                        std::string measurement_type = m_qmap_measurement[ui->measurements_table->item(i,1)->text()].first.toStdString();
                        int measurement_index = m_qmap_measurement[ui->measurements_table->item(i,1)->text()].second;

                        fichier << measurement_name << ";" << measurement_type << ";" << measurement_index << std::endl;
                    }

                    fichier << std::endl;
                    fichier << "end_QMap_measurements" << std::endl << std::endl;

                    fichier.close();
                }
            }
        }
    }
}



void MainWindow::slot_openDistanceSurfaceMeasFromPopup(double _measurement, QString _measurement_type, int _measurement_index)
{
    //m_last_meas_index = _measurement_index;
    m_measurement_form.setDistanceSurfaceMeasValueAndType(_measurement, _measurement_type, _measurement_index);
    m_measurement_form.show();
}


void MainWindow::sl_openInterestPointMeasFromPopup(QString _coordinates, QString _measurement_type, int _measurement_index)
{
    //m_last_meas_index = _measurement_index;
    m_measurement_form.setInterestPointMeasValueAndType(_coordinates, _measurement_type, _measurement_index);
    m_measurement_form.show();
}


void MainWindow::slot_saveMeasFormValues(QString _measur_name, QString _measur_type, QString _category, QString _temperature, QString _measur_result, int _measur_counter, QString _comments)
{
    m_true_counter++;
    m_measurement_index = _measur_counter;

    bool name_is_unique = true;
    bool name_is_empty = false;


    // Check measurement name is not empty and unique and ask again if necessary
    name_is_empty = _measur_name.isEmpty();

    for(int i=1; i < m_true_counter; ++i)
    {
        if(_measur_name == m_qmap_of_names[(m_true_counter-1)-i])
            name_is_unique = false;
    }

    if(name_is_empty || !name_is_unique)
    {
        bool ok;
        if(name_is_empty){
            _measur_name = QInputDialog::getText(this, "Error no entry for measurement name",
                                                 "Sorry, you have not given a name for the measurement, please set one.", QLineEdit::Normal,
                                                 QString(), &ok);
        }
        if(!name_is_unique){
            _measur_name = QInputDialog::getText(this, "Error : non unique name",
                                                 "Sorry, the name you choose for this measurement already exists, please choose another one.", QLineEdit::Normal,
                                                 QString(), &ok);
        }

        // Check again new name
        name_is_empty = _measur_name.isEmpty();
        bool name_is_unique = true;
        for(int i=1; i < m_true_counter; ++i)
        {
            if(_measur_name == m_qmap_of_names[(m_true_counter-1)-i])
                name_is_unique = false;
        }

        if(ok && !name_is_empty && name_is_unique)
        {
            m_qmap_of_names[m_true_counter-1] = _measur_name;
        }
        else
        {
            wrongMeasurName(_measur_type);
            if(name_is_empty)
                QMessageBox::critical(this, "Error : measurement name", "Empty measurement name so this measurement won't be saved.");
            if(!name_is_unique)
                QMessageBox::critical(this, "Error : measurement name", "Measurement name already exist so this measurement won't be saved.");
            m_true_counter--;
            return;
        }
    }
    else
    {
        m_qmap_of_names[m_true_counter-1] = _measur_name;
    }

    // Add measurement to table
    add_measur_to_table(_measur_name, _measur_type, _category, _temperature, _measur_result, _comments);

    ui->measurements_table->resizeColumnsToContents();
}


void MainWindow::add_measur_to_table(QString _measur_name, QString _measur_type, QString _category, QString _temperature, QString _measur_result, QString _comments)
{

    QPair<QString,int> qpair_name_indice_measur;
    qpair_name_indice_measur.first = _measur_type;
    qpair_name_indice_measur.second = m_measurement_index;

    QTableWidgetItem *checkbox = new QTableWidgetItem();
    checkbox->setCheckState(Qt::Checked);

    ui->measurements_table->setRowCount(m_true_counter);

    ui->measurements_table->setItem(m_true_counter-1, 0, checkbox);
    ui->measurements_table->setItem(m_true_counter-1, 1, new QTableWidgetItem(_measur_name));
    ui->measurements_table->setItem(m_true_counter-1, 2, new QTableWidgetItem(_measur_type));
    ui->measurements_table->setItem(m_true_counter-1, 3, new QTableWidgetItem(_category));
    ui->measurements_table->setItem(m_true_counter-1, 4, new QTableWidgetItem(_temperature));
    ui->measurements_table->setItem(m_true_counter-1, 5, new QTableWidgetItem(_measur_result));
    ui->measurements_table->setItem(m_true_counter-1, 6, new QTableWidgetItem(_comments));

    m_qmap_measur_counter[_measur_name] = m_true_counter-1;

    m_qmap_measurement[_measur_name] = qpair_name_indice_measur;
}


void MainWindow::sl_distanceMeasurementFormCanceled()
{
    ui->display_widget->removeLastMeasurementOfType(LINE_MEASUREMENT_STATE);
}

void MainWindow::sl_surfaceMeasurementFormCanceled()
{
    ui->display_widget->removeLastMeasurementOfType(SURFACE_MEASUREMENT_STATE);
}

void MainWindow::sl_interestPointMeasurementFormCanceled()
{
    ui->display_widget->removeLastMeasurementOfType(INTEREST_POINT_STATE);
}



void MainWindow::sl_lineToolActivated()
{
    m_tool_state = LINE_MEASUREMENT_STATE;

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
    m_tool_state = SURFACE_MEASUREMENT_STATE;

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
    m_tool_state = INTEREST_POINT_STATE;

    ui->draw_segment_action->setEnabled(false);
    ui->draw_area_action->setEnabled(false);
    ui->cut_area_action->setEnabled(false);
    ui->draw_interest_point_action->setEnabled(false);
    ui->zoom_in_action->setEnabled(false);
    ui->zoom_out_action->setEnabled(false);
    ui->resize_action->setEnabled(false);
    ui->crop_action->setEnabled(false);
}

void MainWindow::sl_deactivateTool()
{
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


void MainWindow::sl_returnIdleState()
{
    m_tool_state = IDLE_STATE;
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
            ui->display_widget->hideShowMeasurementOfType(m_qmap_convert_state_names[m_qmap_measurement[item_name].first], m_qmap_measurement[item_name].second, false);
        }

        // if item clicked is checked
        else if(_item_clicked->checkState() == Qt::Checked)
        {
            ui->display_widget->hideShowMeasurementOfType(m_qmap_convert_state_names[m_qmap_measurement[item_name].first], m_qmap_measurement[item_name].second, true);
        }
    }
}



void MainWindow::wrongMeasurName(QString _measur_type)
{
    if(_measur_type == "Distance measurement")
    {
        emit si_errorNameDistanceMeasurementFormCanceled();
    }

    else if(_measur_type == "Surface measurement")
    {
        emit si_errorNameSurfaceMeasurementFormCanceled();
    }

    else if(_measur_type == "Interest point measurement")
    {
        emit si_errorNameInterestPointMeasurementFormCanceled();
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
    m_true_counter--;

    int item_row = m_delete_measurement_item->row();
    QString item_name = ui->measurements_table->item(item_row,1)->text();

    ui->display_widget->removeMeasurementOfType(m_qmap_convert_state_names[m_qmap_measurement[item_name].first], m_qmap_measurement[item_name].second);
    ui->measurements_table->removeRow(item_row);
}

