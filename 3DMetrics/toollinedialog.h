#ifndef TOOLINEDIALOG_H
#define TOOLINEDIALOG_H

#include <QDialog>
#include <osg/Geode>

namespace Ui {
class ToolLineDialog;
}

class MeasureLine;

class ToolLineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolLineDialog(QWidget *parent = 0);
    ~ToolLineDialog();

protected:
    void mouseDoubleClickEvent( QMouseEvent * e );
    void closeEvent(QCloseEvent *);
    void reject();

public slots:
    void start();

private:
    Ui::ToolLineDialog *ui;

    MeasureLine *m_measureline;
    osg::ref_ptr<osg::Geode> m_geode;
};

#endif // TOOLINEDIALOG_H
