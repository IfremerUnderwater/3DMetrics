#include "attrib_categories_widget.h"
#include "ui_attrib_categories_widget.h"
#include "Measurement/tdm_categories_manager.h"
#include "Measurement/measurement_category.h"
#include <QDebug>

AttribCategoriesWidget::AttribCategoriesWidget(QWidget *_parent) :
    QWidget(_parent),
    ui(new Ui::AttribCategoriesWidget),
    m_category_item(NULL)
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
    if (m_category_item)
    {
        int idx = ui->categories_cb->findText(m_category_item->value());
        if (idx>0)
            ui->categories_cb->setCurrentIndex(idx);
    }
}

void AttribCategoriesWidget::initItem()
{
    if(m_category_item)
        m_category_item->setValue(ui->categories_cb->currentText());
}


void AttribCategoriesWidget::slot_categoryChanged(QString _new_category)
{
    qDebug() << "Slot called\n";
    if(m_category_item)
        m_category_item->setValue(_new_category);
}
