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
    void slot_toolClickedXY(Point3D&,int, int);
    void slot_mouse_moved(int, int);
    void slot_toolCanceled();
    void slot_toolRemoveLastPoint();

signals:
    void signal_toolStarted(QString &_info);
    void signal_toolEnded(QString &_info);


private:
    Ui::SlopeTool *ui;

    osg::ref_ptr<osg::Geode> m_geode;

    bool m_centerSet;

    osg::Vec3d m_center;
    int m_xcenter;
    int m_ycenter;

    osg::Vec4f m_color;

};

#endif // SLOPE_TOOL_H
