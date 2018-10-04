#ifndef EDIT_MEASURE_DIALOG_H
#define EDIT_MEASURE_DIALOG_H

#include <QDialog>

namespace Ui {
class edit_measure_dialog;
}

class edit_measure_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit edit_measure_dialog(QWidget *parent = 0);
    ~edit_measure_dialog();

private:
    Ui::edit_measure_dialog *ui;
};

#endif // EDIT_MEASURE_DIALOG_H
