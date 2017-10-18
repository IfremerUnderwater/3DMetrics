#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QPair>
#include <QFile>
#include <QAction>
#include <cstring>
#include "measurement_saving_dialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // method to emit a signal when measurement name is false
    void wrongMeasurName(QString _measur_type);

    // add measur to table
    void add_measur_to_table(QString _measur_name, QString _measur_type, QString _category, QString _temperature, QString _measur_result, QString _comments);

public slots:
    void slot_open3dModel();
    void slot_openMeasureFile();
    void slot_close3dModel();
    void sl_saveMeasurFile();

    void slot_openDistanceSurfaceMeasFromPopup(double _measurement, QString _measurement_type, int _measurement_index);
    void sl_openInterestPointMeasFromPopup(QString _coordinates, QString _measurement_type, int _measurement_index);
    void slot_saveMeasFormValues(QString _measur_name, QString _measur_type, QString _category, QString _temperature, QString _measur_result, int _measur_counter, QString _comments);
    void sl_distanceMeasurementFormCanceled();
    void sl_surfaceMeasurementFormCanceled();
    void sl_interestPointMeasurementFormCanceled();

    void sl_lineToolActivated();
    void sl_surfaceToolActivated();
    void sl_interestPointToolActivated();
    void sl_deactivateTool();
    void sl_returnIdleState();

    // slot for show/hide measurement
    void sl_show_hide_measurement(QTableWidgetItem* _item_clicked);
    void sl_contextMenuDeleteMeasurement(const QPoint &pos);

    void sl_delete_measurement_action();


signals:
    void si_state_name(int _state_name);
    void si_errorNameDistanceMeasurementFormCanceled();
    void si_errorNameSurfaceMeasurementFormCanceled();
    void si_errorNameInterestPointMeasurementFormCanceled();
    void si_displayWIdgetInIdleState();


private:
    Ui::MainWindow *ui;
    QString m_model_file;
    QString m_measures_file;
    int m_last_meas_index;
    MeasurementSavingDialog m_measurement_form;
    QMap<int,QString> m_state_names;
    QMap<int,QString> m_qmap_of_names;
    QMap<QString,int> m_qmap_measur_counter;
    int m_true_counter;
    ToolState m_tool_state;

    QMap<QString,QPair<QString,int> > m_qmap_measurement;

    // delete button
    QMap<int,QString> m_qmap_checked_row_counter;
    QMap<QString,ToolState> m_qmap_convert_state_names;

    int m_measurement_index;

    QMenu* m_delete_menu;
    QAction* m_delete_measurement_action;
    QTableWidgetItem* m_delete_measurement_item;

    QString m_position_measur_file;

};

#endif // MAINWINDOW_H
