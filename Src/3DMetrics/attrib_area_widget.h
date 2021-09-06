#ifndef ATTRIB_AREA_WIDGET_H
#define ATTRIB_AREA_WIDGET_H

#include <QWidget>

namespace Ui {
class AttribAreaWidget;
}

class MeasArea;
struct Point3D;

class AttribAreaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttribAreaWidget(QWidget *_parent = 0);
    ~AttribAreaWidget();

    void clicked();

    void setArea(MeasArea *_area_item) {m_area_item = _area_item; update(); }
    // get via table attribute because m_item is a pointer

public slots:
    void slot_toolCanceled();
    void slot_toolEnded();
    void slot_toolClicked(Point3D &_point);
    void slot_toolRemoveLastPoint();

signals:
    void signal_toolStarted(QString &_info);
    void signal_toolEnded(QString &_info);
    void signal_nbPointsChanged();

private:
    void update();

    Ui::AttribAreaWidget *ui;

    MeasArea *m_area_item; //do not delete
};

#endif // ATTRIB_AREA_WIDGET_H
