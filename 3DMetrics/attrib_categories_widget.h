#ifndef ATTRIB_CATEGORIES_WIDGET_H
#define ATTRIB_CATEGORIES_WIDGET_H

#include <QWidget>

namespace Ui {
class AttribCategoriesWidget;
}

class MeasCategory;

class AttribCategoriesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttribCategoriesWidget(QWidget *_parent = 0);
    ~AttribCategoriesWidget();

    void setCategory(MeasCategory *_category) {m_category_item = _category; update();}

    void update();
    void initItem();

private slots:
    void slot_categoryChanged(QString _new_category);

private:
    Ui::AttribCategoriesWidget *ui;
    MeasCategory * m_category_item;

};

#endif // ATTRIB_CATEGORIES_WIDGET_H
