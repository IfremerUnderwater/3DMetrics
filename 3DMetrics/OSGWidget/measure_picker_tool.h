#ifndef MEASUREPICKERTOOL_H
#define MEASUREPICKERTOOL_H

#include <QObject>
#include <osg/Node>

class MeasurePickerTool : public QObject
{
    Q_OBJECT
public:
    explicit MeasurePickerTool(QObject *parent = 0);

public slots:
    void start();
    void slot_toolEnded();
    void slot_toolClicked(int, int);

signals:
    void signal_nodeClicked(osg::Node *_node);
    void signal_noNodeClicked();
};

#endif // MEASUREPICKERTOOL_H


