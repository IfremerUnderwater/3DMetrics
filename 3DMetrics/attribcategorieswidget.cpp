#include "attribcategorieswidget.h"
#include "ui_attribcategorieswidget.h"
#include "Measurement/tdm_categories_manager.h"
#include "Measurement/measurement_category.h"
#include <QDebug>

AttribCategoriesWidget::AttribCategoriesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AttribCategoriesWidget),
    m_item(NULL)
{
    ui->setupUi(this);
    QStringList cat_list = TdmCatManager::instance()->categoriesList();
    int i=0;
    foreach (QString cat, cat_list) {
        ui->categories_cb->insertItem(i,cat);
        i++;
    }

qDebug() << "Arrived in creator\n";
    connect(ui->categories_cb,SIGNAL(currentIndexChanged(QString)),this,SLOT(slot_categoryChanged(QString)));
}

AttribCategoriesWidget::~AttribCategoriesWidget()
{
    delete ui;
}

void AttribCategoriesWidget::update()
{
    if (m_item)
    {
        int idx = ui->categories_cb->findText(m_item->value());
        if (idx>0)
            ui->categories_cb->setCurrentIndex(idx);
    }
}

void AttribCategoriesWidget::initItem()
{
    if(m_item)
        m_item->setValue(ui->categories_cb->currentText());
}


void AttribCategoriesWidget::slot_categoryChanged(QString _new_cat)
{
    qDebug() << "Slot called\n";
    if(m_item)
        m_item->setValue(_new_cat);
}
