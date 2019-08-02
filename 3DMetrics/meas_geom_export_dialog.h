#ifndef MEAS_GEOM_EXPORT_DIALOG_H
#define MEAS_GEOM_EXPORT_DIALOG_H

#include <QDialog>

namespace Ui {
class MeasGeomExportDialog;
}

class MeasGeomExportDialog : public QDialog
{
    Q_OBJECT

public:

    enum export_type {
        ShapeFile = 0,
        ASCII = 1
    };

    explicit MeasGeomExportDialog(QWidget *parent = 0);
    ~MeasGeomExportDialog();
    QString getFilename() { return m_filename; }
    bool getPointSelected() { return m_point_selected; }
    bool getLineSelected() { return m_line_selected; }
    bool getAreaSelected() { return m_area_selected; }
    MeasGeomExportDialog::export_type getExportType() { return m_export_type; }
    bool getXYZSelected() { return m_xyz_selected; }
    bool getLatLonSelected() { return m_lat_lon_selected; }
    void clear();

private:
    Ui::MeasGeomExportDialog *ui;

    QString m_filename;
    bool m_point_selected;
    bool m_line_selected;
    bool m_area_selected;
    MeasGeomExportDialog::export_type m_export_type;
    bool m_xyz_selected;
    bool m_lat_lon_selected;

private slots:
    void slot_selectPath();
    void on_ASCII_btn_clicked();
    void on_shapefile_btn_clicked();
    void slot_apply();
    void slot_textFilenameChanged(QString);
    void slot_asciiChecked(bool);

};

#endif // MEAS_GEOM_EXPORT_DIALOG_H
