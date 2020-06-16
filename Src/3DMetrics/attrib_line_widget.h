#ifndef ATTRIB_LINE_WIDGET_H
#define ATTRIB_LINE_WIDGET_H

#include <QWidget>
#include <QShortcut>

class Point3D;

namespace Ui {
class AttribLineWidget;
}

class MeasLine;

class AttribLineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttribLineWidget(QWidget *_parent = 0);
    ~AttribLineWidget();

    void clicked();

    void setLine(MeasLine *_line) {m_line_item = _line; update(); }
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

    Ui::AttribLineWidget *ui;
    MeasLine *m_line_item; //do not delete
};

#endif // ATTRIB_LINE_WIDGET_H
