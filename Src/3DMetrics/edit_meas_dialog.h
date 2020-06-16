#ifndef EDIT_MEAS_DIALOG_H
#define EDIT_MEAS_DIALOG_H

#include <QDialog>
#include <QJsonDocument>
#include "Measurement/measurement_pattern.h"

namespace Ui {
class EditMeasDialog;
}

class QListWidgetItem;
class EditMeasItem;

class EditMeasDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditMeasDialog(QWidget *_parent = 0);
    ~EditMeasDialog();

    void setPattern(MeasPattern _pattern);

public slots:
    void slot_addItem();
    void slot_remove(QListWidgetItem*);
    void slot_loadPattern();
    void slot_savePattern();
    void slot_apply();

signals:
    void signal_apply(MeasPattern _pattern);

private:
    Ui::EditMeasDialog *ui;

    EditMeasItem* addItem();
    MeasPattern createPattern();
};

#endif // EDIT_MEAS_DIALOG_H
