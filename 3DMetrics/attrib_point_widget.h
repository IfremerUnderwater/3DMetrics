#ifndef ATTRIB_POINT_WIDGET_H
#define ATTRIB_POINT_WIDGET_H

#include <QWidget>

namespace Ui {
class AttribPointWidget;
}

class MeasPoint;
class Point3D;

class AttribPointWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttribPointWidget(QWidget *_parent = 0);
    ~AttribPointWidget();

    void clicked();

    void setPoint(MeasPoint *_point, bool _value_ok = true) {m_point_item = _point; update(_value_ok); }
    // get via table attribute because m_item is a pointer

public slots:
    //void slot_toolCanceled(); // ended used - no need to cancel
    void slot_toolEnded();
    void slot_toolClicked(Point3D &_point);

signals:
    void signal_toolStarted(QString &_info);
    void signal_toolEnded(QString &_info);

private:
    void update(bool _value_ok = true);

    Ui::AttribPointWidget *ui;
    MeasPoint *m_point_item; //do not delete
};

#endif // ATTRIB_POINT_WIDGET_H
