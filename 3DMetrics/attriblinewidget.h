#ifndef ATTRIBLINEWIDGET_H
#define ATTRIBLINEWIDGET_H

#include <QWidget>

namespace Ui {
class AttribLineWidget;
}

class MeasureLine;

class AttribLineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttribLineWidget(QWidget *parent = 0);
    ~AttribLineWidget();

//    void setNbval(QString _nb);
//    void setLengthval(QString _length);

    void clicked();

    void setLine(MeasureLine *_p) {m_item = _p; update(); }
    // get via table attribute because m_item is a pointer

private:
    void update();

    Ui::AttribLineWidget *ui;
    MeasureLine *m_item; //do not delete
};

#endif // ATTRIBLINEWIDGET_H
