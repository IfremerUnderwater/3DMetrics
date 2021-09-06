#ifndef EDIT_LOD_THRESHOLD_DIALOG_H
#define EDIT_LOD_THRESHOLD_DIALOG_H

#include <QDialog>

namespace Ui {
class EditLODThresholdDialog;
}

class EditLODThresholdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditLODThresholdDialog(QWidget *parent = 0);
    ~EditLODThresholdDialog();


    double getThreshold1() const { return m_threshold1; }
    double getThreshold2() const { return m_threshold2; }

    void setThresholds(const double _threshold1, const double _threshold2);

public slots:
    void slot_changeTh1(double _val);
    void slot_changeTh2(double _val);

    void slot_ok();
private:
    Ui::EditLODThresholdDialog *ui;

    double m_threshold1;
    double m_threshold2;
};

#endif // EDIT_LOD_THRESHOLD_DIALOG_H
