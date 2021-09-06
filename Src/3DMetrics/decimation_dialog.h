#ifndef DECIMATION_DIALOG_H
#define DECIMATION_DIALOG_H

#include <QDialog>

namespace Ui {
class DecimationDialog;
}

class DecimationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DecimationDialog(QWidget *_parent = 0);
    ~DecimationDialog();

    QString getModelPath();
    double getDecimationFactor();

private:
    Ui::DecimationDialog *ui;

private slots:
    void slot_selectModel();
};

#endif // DECIMATION_DIALOG_H
