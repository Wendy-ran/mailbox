#ifndef NEWDATABASE_H
#define NEWDATABASE_H
#include <QString>
#include <QTreeView>

class newDatabase
{
public:
    newDatabase();
    ~newDatabase();

    bool openDB(QString dbFilePath);
    void setupModelAndView(QTreeView *view, QString tableName);
};

#endif // NEWDATABASE_H
