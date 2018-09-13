#include "tdmgui.h"
#include "ui_tdmgui.h"

//#include "TDMLayers/tdmlayer.h"
//#include "TDMLayers/tdmmaplayerregistry.h"

#include "TreeView/treeitem.h"
#include "TreeView/treemodel.h"

TDMGui::TDMGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TDMGui)
{
    ui->setupUi(this);
//    TDMLayerRegistry *reg = TDMLayerRegistry::instance();

//    reg->addMapLayer(new TDMLayer(TDMLayer::ModelLayer, tr("Models")));

//    ui->tree_widget->setModel(reg);

    ui->tree_widget->setModel(TreeModel::instance());
    ui->tree_widget->hideColumn(1);

    QObject::connect(ui->open_3d_model_action, SIGNAL(triggered()), this, SLOT(slot_open3dModel()));
    QObject::connect(ui->open_measurement_file_action, SIGNAL(triggered()), this, SLOT(slot_openMeasureFile()));
    QObject::connect(ui->new_group_action, SIGNAL(triggered()), this, SLOT(slot_newGroup()));

    // check state
    QObject::connect(TreeModel::instance(),SIGNAL(signal_checkChanged(TreeItem*)),this,SLOT(slot_checkChanged(TreeItem*)));
}

TDMGui::~TDMGui()
{
    delete ui;
}

void TDMGui::slot_open3dModel()
{

    // Problem on Linux Ubuntu : to be replaced
    //
    //    QString model_file = QFileDialog::getOpenFileName(
    //                this,
    //                "Select one 3d Model to open");

    QFileDialog fileDialog(this,tr("Select one 3d Model to open"), "", tr("3D files (*.kml *.obj)"));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFiles);

    fileDialog.setOption(QFileDialog::DontUseCustomDirectoryIcons, true);

#ifndef __MINGW32__
    // Ubuntu Linux doesn't show the dialog properly without this option
    fileDialog.setOption(QFileDialog::DontUseNativeDialog,true);
#endif

    if (QDialog::Accepted != fileDialog.exec())
        return;


    QStringList model_file = fileDialog.selectedFiles();
    if(model_file.count() > 0)
    {
        QString fileName = model_file.first();
        TreeModel *model = TreeModel::instance(); //static_cast<TreeModel*>(ui->tree_widget->model());
        QFileInfo info(fileName);
        QVariant data(info.fileName());
        QVariant dummy(fileName);
//        QVector<QVariant> datas;
//        datas << data;
//        datas << dummy;
        TreeItem *added = model->addData(TreeItem::ModelLayer, model->rootItem(), data, dummy);
        added->setChecked(true);

        ui->display_widget->setSceneFromFile(fileName.toStdString());

//        // test
//        dummy.setValue(QString("dummy group"));
//        model->addData(model->getGroupsItem(), dummy, dummy);

//        // test
//        dummy.setValue(QString("dummy measure"));
//        model->addData(model->getMeasuresItem(), dummy, dummy);
     }
}

void TDMGui::slot_openMeasureFile()
{
    TreeModel *model = TreeModel::instance();
    QVariant data("Dummy MeasureFile");
    QVariant dummy("dummy");
    TreeItem *added = model->addData(TreeItem::MeasurementLayer, model->rootItem(), data, dummy);
    added->setChecked(true);
}

void TDMGui::slot_newGroup()
{
    TreeModel *model = TreeModel::instance();
    QVariant data("Group");
    QVariant dummy("group");
    TreeItem *added = model->addData(TreeItem::GroupLayer, model->rootItem(), data, dummy);
    added->setChecked(false);
}

void TDMGui::slot_checkChanged(TreeItem *item)
{
    bool checked = item->isChecked();
}
