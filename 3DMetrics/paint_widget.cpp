#include "paint_widget.h"

#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QMouseEvent>

PaintWidget::PaintWidget(QWidget *parent) : QWidget(parent)
{
    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
}

void PaintWidget::paintEvent(QPaintEvent * /*event*/)
{
    //create a QPainter and pass a pointer to the device.
    //A paint device can be a QWidget, a QPixmap or a QImage
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    float xmin = 0;
    float xmax = 0;

    float ymin = 0;
    float ymax = 0;

    int npts = m_mainPts.size();

    if(npts > 0)
    {
        xmin = m_mainPts[0].x();
        xmax = m_mainPts[npts - 1].x();

        ymin = m_mainPts[0].y();
        ymax = m_mainPts[0].y();
        for(int i=0; i<npts; i++)
        {
            float y = m_mainPts[i].y();
            if( y < ymin)
                ymin = y;
            if( y > ymax)
                ymax = y;
        }
    }
    else
        return; // nothing to draw

    if(npts == 1)
        return; // nothing to draw

    // compute ymin / ymax from model points
    for(int i=0; i<m_modelPts.size(); i++)
    {
        float y = m_modelPts[i].y();
        if( y < ymin)
            ymin = y;
        if( y > ymax)
            ymax = y;
    }

    m_xmin = xmin;
    m_xmax = xmax;
    m_ymin = ymin;
    m_ymax = ymax;

    int marginX = 25;
    int w = width() - 2*marginX;
    int marginY = 15;
    int h = height() - 2*marginY;


    QPen pen(Qt::gray, 1, Qt::SolidLine);
    painter.setPen(pen);

    // draw regtangle
    painter.drawRect(marginX, marginY,w,h);

    pen.setColor(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);

    // draw lines
    int x0 = marginX + w * (m_mainPts[0].x() - xmin) / (xmax - xmin);
    int y0 = height() - marginY - h * (m_mainPts[0].y() - ymin) / (ymax - ymin);

    for(int i=1; i<npts; i++)
    {
        int x = marginX + w * (m_mainPts[i].x() - xmin) / (xmax - xmin);
        int y = height() - marginY - h * (m_mainPts[i].y() - ymin) / (ymax - ymin);
        painter.drawLine(x0,y0,x,y);
        x0 = x;
        y0 = y;
    }

    // draw model
    pen.setColor(Qt::blue);
    painter.setPen(pen);
    QPolygon polygon;

    x0 = marginX + w * (m_modelPts[0].x() - xmin) / (xmax - xmin);
    y0 = height() - marginY - h * (m_modelPts[0].y() - ymin) / (ymax - ymin);

    polygon << QPoint(x0, height() - marginY);
    polygon << QPoint(x0, y0);

    for(int i=1; i<m_modelPts.size(); i++)
    {
        int x = marginX + w * (m_modelPts[i].x() - xmin) / (xmax - xmin);
        int y = height() - marginY - h * (m_modelPts[i].y() - ymin) / (ymax - ymin);
        painter.drawLine(x0,y0,x,y);
        x0 = x;
        y0 = y;
        polygon << QPoint(x0, y0);
    }
    polygon << QPoint(x0, height() - marginY);
    QBrush brush;
    QColor blue = Qt::blue;
    blue.setAlphaF(0.4);
    brush.setColor(blue);
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.drawPolygon(polygon);

    // draw points
    pen.setColor(Qt::red);
    pen.setWidth(5);
    painter.setPen(pen);
    for(int i=0; i<npts; i++)
    {
        int x = marginX + w * (m_mainPts[i].x() - xmin) / (xmax - xmin);
        int y = height() - marginY - h * (m_mainPts[i].y() - ymin) / (ymax - ymin);
        painter.drawPoint(x, y);
    }

    // legend
    pen.setColor(Qt::gray);
    painter.setPen(pen);
    QFont font = painter.font();
    font.setPixelSize(8);

    painter.drawText(marginX, height(), "0.0 m");

    QString max = QString::number(m_mainPts[m_mainPts.size()-1].x() ,'f',1);
    painter.drawText(width() - marginX-50, height(), max + " m");

    max = QString::number(ymax ,'f',1);
    painter.rotate(-90.0);
    painter.drawText(-marginY-60, marginX-3, max + " m");

    QString min = QString::number(ymin ,'f',1);
    painter.drawText(-height() + marginY/2, marginX-3, min + " m");
}

void PaintWidget::setMainPolyLine(QVector<QPointF>  _pts)
{
    m_mainPts = _pts;
}

void PaintWidget::setModelPolyLine(QVector<QPointF>  _pts)
{
    m_modelPts = _pts;
}

void PaintWidget::mouseMoveEvent(QMouseEvent *event)
{
    int marginX = 25;
    int marginY = 15;

    float x = (event->x()-marginX) * (m_xmax - m_xmin) / (width() - 2*marginX);
    float y = (height() - marginY -event->y()) * (m_ymax - m_ymin) / (height() - 2*marginY);

    emit mousemoved(x, y + m_ymin);
}

void PaintWidget::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
}

void PaintWidget::leaveEvent(QEvent *event)
{
    emit mouseleaved();

    QWidget::leaveEvent(event);
}
