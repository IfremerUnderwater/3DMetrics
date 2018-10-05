#ifndef ATTRIBAREAWIDGET_H
#define ATTRIBAREAWIDGET_H

#include <QWidget>

namespace Ui {
class AttribAreaWidget;
}

class AttribAreaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttribAreaWidget(QWidget *parent = 0);
    ~AttribAreaWidget();


    void setNbval(QString _nb);
    void setAreaval(QString _area);

public slots:
    void slot_clicked();

private:
    Ui::AttribAreaWidget *ui;
};

#endif // ATTRIBAREAWIDGET_H
