#ifndef ATTRIBPOINTWIDGET_H
#define ATTRIBPOINTWIDGET_H

#include <QWidget>

namespace Ui {
class AttribPointWidget;
}

class MeasurePoint;
class Point3D;

class AttribPointWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttribPointWidget(QWidget *parent = 0);
    ~AttribPointWidget();

    void clicked();

    void setPoint(MeasurePoint *_p, bool valueOk = true) {m_item = _p; update(valueOk); }
    // get via table attribute because m_item is a pointer

public slots:
    //void slot_toolCanceled(); // ended used
    void slot_toolEnded();
    void slot_toolApply(Point3D &p);

signals:
    void signal_toolStarted(QString &info);
    void signal_toolEnded(QString &info);

private:

    void update(bool valueOk = true);

    Ui::AttribPointWidget *ui;
    MeasurePoint *m_item; //do not delete
};

#endif // ATTRIBPOINTWIDGET_H
