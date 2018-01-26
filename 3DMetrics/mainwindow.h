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
#include "tool_handler.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // add measur to table
    void addMeasToTable(QString _measur_name, QString _measur_type, QString _category, QString _temperature, QString _measur_result, QString _comments);

    void goBackToIdle();

public slots:
    void slot_open3dModel();
    void slot_openMeasureFile();
    void slot_close3dModel();
    void sl_saveMeasurFile();

    void slot_openMeasSavingPopup();
    void sl_formSavingCanceled();

    //
    void sl_lineToolActivated();
    void sl_surfaceToolActivated();
    void sl_interestPointToolActivated();
    void sl_cancelMeasurement();

    // slot for showing/hiding measurement
    void sl_show_hide_measurement(QTableWidgetItem* _item_clicked);
    void sl_contextMenuDeleteMeasurement(const QPoint &pos);

    void sl_delete_measurement_action();

    // slot for reading and saving form values
    void slot_saveMeasFormValuesToTable();
signals:


private:
    Ui::MainWindow *ui;
    QString m_model_file;
    QString m_measures_file;

    MeasurementSavingDialog m_measurement_form;
    QMap<int,QString> m_state_names;
    QMap<int,QString> m_qmap_of_names;
    QMap<QString,int> m_qmap_measur_counter;


    ToolHandler *m_tool_handler;
    QMap<ToolState,QString> m_toolstate_to_qstring;

    QMap<QString,QPair<ToolState,int> > m_qmap_measurement;

    // delete button
    QMap<int,QString> m_qmap_checked_row_counter;
    QMap<QString,ToolState> m_qmap_convert_state_names;

    QMenu* m_delete_menu;
    QAction* m_delete_measurement_action;
    QTableWidgetItem* m_delete_measurement_item;

    QString m_position_measur_file;

};

#endif // MAINWINDOW_H
