#ifndef ATTRIBPOINTWIDGET_H
#define ATTRIBPOINTWIDGET_H

#include <QWidget>

namespace Ui {
class AttribPointWidget;
}

class MeasurePoint;

class AttribPointWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttribPointWidget(QWidget *parent = 0);
    ~AttribPointWidget();

    void clicked();

    void setPoint(MeasurePoint *_p) {m_item = _p; update(); }
    // get via table attribute because m_item is a pointer

//public slots:
//    void slot_clicked();

private:
//    void setXval(QString _x);
//    void setYval(QString _y);
//    void setZval(QString _z);

    void update();

    Ui::AttribPointWidget *ui;
    MeasurePoint *m_item; //do not delete
};

#endif // ATTRIBPOINTWIDGET_H
