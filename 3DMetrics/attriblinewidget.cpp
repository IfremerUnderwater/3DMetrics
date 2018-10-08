#include "attriblinewidget.h"
#include "ui_attriblinewidget.h"

#include "Measure/measureline.h"

AttribLineWidget::AttribLineWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AttribLineWidget)
{
    ui->setupUi(this);

    //QObject::connect(ui->tool_btn, SIGNAL(clicked(bool)), this, SLOT(slot_clicked()));

    ui->tool_label->setStyleSheet("background-color: red");
}

AttribLineWidget::~AttribLineWidget()
{
    delete ui;
}

//void AttribLineWidget::setNbval(QString _nb)
//{
//    ui->pts_label->setText(_nb);
//}

//void AttribLineWidget::setLengthval(QString _length)
//{
//    ui->length_label->setText(_length);
//}

void AttribLineWidget::clicked()
{
    ui->tool_label->setStyleSheet("");

    if(m_item)
    {
        // test
        double t = m_item->getArray().length();
        Point3D pt;
        pt.x = t;
        pt.y = t;
        pt.z = t;
        m_item->getArray().append(pt);
        m_item->computeLength();
        update();
    }

}

void AttribLineWidget::update()
{
    if(m_item)
    {
        QString snb = QString::number(m_item->nbPts());
        ui->pts_label->setText(snb);
        QString slg = QString::number(m_item->length(),'f',2);
        ui->length_label->setText(slg);
    }
}

