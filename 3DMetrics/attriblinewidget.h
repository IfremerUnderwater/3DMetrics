#ifndef ATTRIBLINEWIDGET_H
#define ATTRIBLINEWIDGET_H

#include <QWidget>

namespace Ui {
class AttribLineWidget;
}

class AttribLineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttribLineWidget(QWidget *parent = 0);
    ~AttribLineWidget();

private:
    Ui::AttribLineWidget *ui;
};

#endif // ATTRIBLINEWIDGET_H
