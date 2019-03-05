#ifndef CategoriesManager_H
#define CategoriesManager_H

#include <QString>
#include <QStringList>

class TdmCatManager
{

public:
    // singleton
    static TdmCatManager *instance() { return s_instance; }

    ~TdmCatManager();

    QStringList categoriesList() const;

private:
    // singleton
    TdmCatManager(QString _cat_file);
    static TdmCatManager *s_instance;

    QStringList m_categories_list;

};

#endif // CategoriesManager_H
