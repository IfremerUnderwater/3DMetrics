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

private:
    Ui::AttribPointWidget *ui;
};

#endif // ATTRIBPOINTWIDGET_H
