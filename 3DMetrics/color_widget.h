#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include <QWidget>

class ColorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorWidget(QWidget *parent = 0);

    void redraw();

    // accessors
    double zmin() const
    {
        return m_zmin;
    }

    void setZmin(double zmin)
    {
        m_zmin = zmin;
    }

    double zmax() const
    {
        return m_zmax;
    }

    void setZmax(double zmax)
    {
        m_zmax = zmax;
    }

    double edit_zmin() const
    {
        return m_edit_zmin;
    }

    void setEdit_zmin(double edit_zmin)
    {
        m_edit_zmin = edit_zmin;
    }
    double edit_zmax() const
    {
        return m_edit_zmax;
    }

    void setEdit_zmax(double edit_zmax)
    {
        m_edit_zmax = edit_zmax;
    }



protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private:
    double m_zmin;
    double m_zmax;

    double m_edit_zmin;
    double m_edit_zmax;

};

#endif // COLORWIDGET_H
