#ifndef ATTRIBCATEGORIESWIDGET_H
#define ATTRIBCATEGORIESWIDGET_H

#include <QWidget>

namespace Ui {
class AttribCategoriesWidget;
}

class MeasureCategory;

class AttribCategoriesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttribCategoriesWidget(QWidget *parent = 0);
    ~AttribCategoriesWidget();

    void setCategory(MeasureCategory *_c) {m_item = _c; update();}

    void update();
    void initItem();

private slots:
    void slot_categoryChanged(QString _new_cat);

private:
    Ui::AttribCategoriesWidget *ui;
    MeasureCategory * m_item;

};

#endif // ATTRIBCATEGORIESWIDGET_H
