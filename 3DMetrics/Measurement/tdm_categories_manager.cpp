#include "tdm_categories_manager.h"
#include <fstream>
#include <iostream>
#include <QDebug>

using namespace std;

TdmCatManager *TdmCatManager::s_instance = new TdmCatManager(QString("categories.txt"));

TdmCatManager::TdmCatManager(QString _cat_file)
{
    // read categories
    string line;
    ifstream cat_file (_cat_file.toStdString());
    if (cat_file)
    {
        int i=0;
        while (getline( cat_file, line ) )
        {
            m_categories_list.push_back(QString::fromStdString(line));
            i++;
        }
        cat_file.close();
    }
    else qDebug() << "Unable to open file";
}

QStringList TdmCatManager::categoriesList() const
{
    return m_categories_list;
}

TdmCatManager::~TdmCatManager()
{
}
