#include "color_widget.h"
#include <QPainter>

#include <osg/Vec3f>

static const int MARGINX = 65; // left and right
static const int MARGINY = 20; // top only
static const int LEGENDY = 10;

ColorWidget::ColorWidget(QWidget *parent) : QWidget(parent)
{

}

void ColorWidget::redraw()
{

}

static osg::Vec3f HSVSpectrum(float x)
{
    float y = 1.0;
    float z = 1.0;
    osg::Vec3f RGB(x, y, z);
    float hi = floor(x * 6.0);
    float f = x * 6.0 - hi;
    float p = z * (1.0-y);
    float q = z * (1.0-y*f);
    float t = z * (1.0-y*(1.0-f));
    if(y != 0.0)
    {
        if (hi == 0.0 || hi == 6.0) { RGB = osg::Vec3f(z, t, p); }
        else if (hi == 1.0) { RGB = osg::Vec3f(q, z, p); }
        else if (hi == 2.0) { RGB = osg::Vec3f(p, z, t); }
        else if (hi == 3.0) { RGB = osg::Vec3f(p, q, z); }
        else if (hi == 4.0) { RGB = osg::Vec3f(t, p, z); }
        else { RGB = osg::Vec3f(z, p, q); }
    }
    return RGB;

}

void ColorWidget::paintEvent(QPaintEvent * /*event*/)
{
    //create a QPainter and pass a pointer to the device.
    //A paint device can be a QWidget, a QPixmap or a QImage
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    double zmin = m_zmin;
    if(m_edit_zmin < zmin)
        zmin = m_edit_zmin;
    if(m_zmax < zmin)
        zmin = m_zmax;
    if(m_edit_zmax < zmin)
        zmin = m_edit_zmax;

    double zmax = m_zmax;
    if(m_edit_zmax > zmax)
        zmax = m_edit_zmax;
    if(m_zmin > zmax)
        zmax = m_zmin;
    if(m_edit_zmin > zmax)
        zmax = m_edit_zmin;

    double deltaz = zmax - zmin;

    int w = width() - 2*MARGINX;
    int h = height() - MARGINY;

    if(deltaz == 0)
        return;

    float zminpos = h * (m_zmin - zmin) / deltaz;
    float zmaxpos = h * (m_zmax - zmin) / deltaz;

    float ezminpos = h * (m_edit_zmin - zmin) / deltaz;
    float ezmaxpos = h * (m_edit_zmax - zmin) / deltaz;

    if(ezmaxpos - ezminpos == 0)
        return;

    // center : rainbow
    for(int y=0; y<=h; y++)
    {
        float z = zmin + y* deltaz / h;

        float val = 0;
        if(z <= m_edit_zmin)
            val = 0;
        else if(z >= m_edit_zmax)
            val = 1.0;
        else
        {
            val = (z - m_edit_zmin) / (m_edit_zmax - m_edit_zmin);
        }

        float v2 = (-val * 0.75) + 0.67;
        if(v2 > 1.0)
            v2 = v2- 1.0;
        osg::Vec3f RGB = HSVSpectrum(v2);
        QColor color(RGB[0]*255, RGB[1]*255, RGB[2]*255);
        // draw line
        QPen pen(color, 1, Qt::SolidLine);
        painter.setPen(pen);
        painter.drawLine(MARGINX,height()-y , w+MARGINX, height()-y );
    }

    // legend
    QPen pen(Qt::black, 1, Qt::SolidLine);
    painter.setPen(pen);
    QFont font = painter.font();
    font.setPixelSize(7);

    painter.drawText(0, LEGENDY , "Models");
    painter.drawText(w+MARGINX, LEGENDY , "Edit");

    // left : Zmin and Zmax

    QString min = QString::number(m_zmin ,'f',1);
    painter.drawText(0, height() - zminpos , min + " m");

    QString max = QString::number(m_zmax ,'f',1);
    painter.drawText(0, height() - zmaxpos , max + " m");

    // right : edited zmin and zmax
    min = QString::number(m_edit_zmin ,'f',1);
    painter.drawText(w+MARGINX, height() - ezminpos , min + " m");

    max = QString::number(m_edit_zmax ,'f',1);
    painter.drawText(w+MARGINX, height() - ezmaxpos , max + " m");
}
