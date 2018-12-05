#ifndef EDIT_MEASURE_DIALOG_H
#define EDIT_MEASURE_DIALOG_H

#include <QDialog>
#include <QJsonDocument>
#include "Measurement/measurement_pattern.h"

namespace Ui {
class edit_measure_dialog;
}

class QListWidgetItem;
class edit_measure_item;

class edit_measure_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit edit_measure_dialog(QWidget *parent = 0);
    ~edit_measure_dialog();

    void setPattern(MeasurePattern pattern);

public slots:
    void slot_addItem();
    void slot_remove(QListWidgetItem*);
    void slot_loadPattern();
    void slot_savePattern();
    void slot_apply();

signals:
    void signal_apply(MeasurePattern pattern);

private:
    Ui::edit_measure_dialog *ui;

    edit_measure_item* addItem();
    MeasurePattern createPattern();
};

#endif // EDIT_MEASURE_DIALOG_H
