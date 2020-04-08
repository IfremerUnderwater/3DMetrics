#include "measure_picker_dialog.h"
#include "ui_measure_picker_dialog.h"

#include "OSGWidget/osg_widget.h"
#include "OSGWidget/osg_widget_tool.h"

MeasurePickerDialog::MeasurePickerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurePickerDialog)
{
    ui->setupUi(this);


    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));

    start();
}

MeasurePickerDialog::~MeasurePickerDialog()
{
    delete ui;
}

void MeasurePickerDialog::mouseDoubleClickEvent( QMouseEvent * _e )
{
    if ( _e->button() == Qt::LeftButton )
    {
        start();
    }
}

void MeasurePickerDialog::closeEvent(QCloseEvent *)
{
    slot_toolEnded();
}

void MeasurePickerDialog::reject()
{
    slot_toolEnded();
    close();
}

void MeasurePickerDialog::start()
{

    OSGWidgetTool *tool = OSGWidgetTool::instance();
    tool->slot_cancelTool();

    // start tool
    QString msg = "Measure Picker tool started";
    emit signal_toolStarted(msg);

    connect(tool, SIGNAL(signal_clickedLMouse( int, int)), this, SLOT(slot_toolClicked( int, int)));
    connect(tool, SIGNAL(signal_endTool()), this, SLOT(slot_toolEnded()));
    connect(tool, SIGNAL(signal_cancelTool()), this, SLOT(slot_toolCanceled()));

    tool->startTool(OSGWidgetTool::MeasurePicker);

}

void MeasurePickerDialog::slot_toolEnded()
{
    disconnect(OSGWidgetTool::instance(), 0, this, 0);
    disconnect(OSGWidgetTool::instance()->getOSGWidget(), 0, this, 0);

    OSGWidgetTool::instance()->endTool();


    QString msg = "Measure Picker ended";
    emit signal_toolEnded(msg);
}

void MeasurePickerDialog::slot_toolClicked(int _x, int _y)
{
    // TODO
    OSGWidget* w = OSGWidgetTool::instance()->getOSGWidget();

    bool exist;
    osg::ref_ptr<osg::Node> node;
    w->getIntersectionPointNode(_x,_y, node, exist);
    if(exist)
    {
        emit signal_nodeClicked(node.get());
    }
}

void MeasurePickerDialog::slot_toolCanceled()
{
    ui->message_label->setText("");
}
