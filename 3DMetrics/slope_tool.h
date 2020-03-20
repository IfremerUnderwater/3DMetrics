#ifndef SLOPE_TOOL_H
#define SLOPE_TOOL_H

#include <QDialog>
#include <osg/Geode>

namespace Ui {
class SlopeTool;
}

class Point3D;

class SlopeTool : public QDialog
{
    Q_OBJECT

public:
    explicit SlopeTool(QWidget *parent = 0);
    ~SlopeTool();

protected:
    void mouseDoubleClickEvent( QMouseEvent * _e );
    void closeEvent(QCloseEvent *);
    void reject();

public slots:
    void start();
    void slot_toolEnded();
    void slot_toolClicked(Point3D&);

signals:
    void signal_toolStarted(QString &_info);
    void signal_toolEnded(QString &_info);

private:
    Ui::SlopeTool *ui;

    osg::ref_ptr<osg::Geode> m_geode;

    bool centerSet;

    double m_centerX;
    double m_centerY;
    double m_centerZ;

    osg::Vec4f m_color;

};

#endif // SLOPE_TOOL_H
