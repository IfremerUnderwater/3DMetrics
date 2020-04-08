#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>

class PaintWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PaintWidget(QWidget *parent = 0);

    void setMainPolyLine(QVector<QPointF>  _pts);
    void setModelPolyLine(QVector<QPointF>  _pts);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

signals:
    void mousemoved(float _x, float _y);
    void mouseleaved();

public slots:

private:
    QVector<QPointF> m_mainPts;
    QVector<QPointF> m_modelPts;

    float m_xmin;
    float m_xmax;
    float m_ymin;
    float m_ymax;
};

#endif // PAINTWIDGET_H
