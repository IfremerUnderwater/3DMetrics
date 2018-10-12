#ifndef ATTRIBLINEWIDGET_H
#define ATTRIBLINEWIDGET_H

#include <QWidget>

class Point3D;

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

    void clicked();

    void setLine(MeasureLine *_p) {m_item = _p; update(); }
    // get via table attribute because m_item is a pointer

public slots:
    //void slot_toolCanceled(); // ended used
    void slot_toolEnded();
    void slot_toolClicked(Point3D &p);

signals:
    void signal_toolStarted(QString &info);
    void signal_toolEnded(QString &info);

private:
    void update();

    Ui::AttribLineWidget *ui;
    MeasureLine *m_item; //do not delete
};

#endif // ATTRIBLINEWIDGET_H
