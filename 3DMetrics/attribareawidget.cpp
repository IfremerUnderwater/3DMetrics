#include "attribareawidget.h"
#include "ui_attribareawidget.h"
#include "Measure/measurearea.h"

AttribAreaWidget::AttribAreaWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AttribAreaWidget)
{
    ui->setupUi(this);

    //QObject::connect(ui->tool_btn, SIGNAL(clicked(bool)), this, SLOT(slot_clicked()));

    ui->tool_label->setStyleSheet("background-color: red");
}

AttribAreaWidget::~AttribAreaWidget()
{
    delete ui;
}


//void AttribAreaWidget::setNbval(QString _nb)
//{
//    ui->pts_label->setText(_nb);
//}

//void AttribAreaWidget::setAreaval(QString _area)
//{
//    ui->length_label->setText(_area);
//}

void AttribAreaWidget::clicked()
{
    ui->tool_label->setStyleSheet("");
    if(m_item)
    {
        //*** TODO

    }
}

void AttribAreaWidget::update()
{

    if(m_item)
    {
        QString snb = QString::number(m_item->nbPts());
        ui->pts_label->setText(snb);
        QString slg = QString::number(m_item->area(),'f',2);
        ui->length_label->setText(slg);
    }
}
