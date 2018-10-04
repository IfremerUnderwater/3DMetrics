#ifndef EDIT_MEASURE_ITEM_H
#define EDIT_MEASURE_ITEM_H

#include <QWidget>

namespace Ui {
class edit_measure_item;
}

class edit_measure_item : public QWidget
{
    Q_OBJECT

public:
    explicit edit_measure_item(QWidget *parent = 0);
    ~edit_measure_item();

private:
    Ui::edit_measure_item *ui;
};

#endif // EDIT_MEASURE_ITEM_H
