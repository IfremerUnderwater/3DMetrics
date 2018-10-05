#ifndef ATTRIBPOINTWIDGET_H
#define ATTRIBPOINTWIDGET_H

#include <QWidget>

namespace Ui {
class AttribPointWidget;
}

class AttribPointWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttribPointWidget(QWidget *parent = 0);
    ~AttribPointWidget();

    void setXval(QString _x);
    void setYval(QString _y);
    void setZval(QString _z);

public slots:
    void slot_clicked();

private:
    Ui::AttribPointWidget *ui;
};

#endif // ATTRIBPOINTWIDGET_H
