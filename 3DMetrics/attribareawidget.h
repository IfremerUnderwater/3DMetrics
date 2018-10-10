#ifndef ATTRIBAREAWIDGET_H
#define ATTRIBAREAWIDGET_H

#include <QWidget>

namespace Ui {
class AttribAreaWidget;
}

class MeasureArea;

class AttribAreaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttribAreaWidget(QWidget *parent = 0);
    ~AttribAreaWidget();

    void clicked();

    void setArea(MeasureArea *_a) {m_item = _a; update(); }
    // get via table attribute because m_item is a pointer

private:
    void update();

    Ui::AttribAreaWidget *ui;


    MeasureArea *m_item; //do not delete

};

#endif // ATTRIBAREAWIDGET_H
