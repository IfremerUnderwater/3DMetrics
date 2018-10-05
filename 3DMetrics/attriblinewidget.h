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

    void setNbval(QString _nb);
    void setLengthval(QString _length);

public slots:
    void slot_clicked();

private:
    Ui::AttribLineWidget *ui;
};

#endif // ATTRIBLINEWIDGET_H
