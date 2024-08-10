#include "newdatabase.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

newDatabase::newDatabase() {

}

newDatabase::~newDatabase()
{

}

bool newDatabase::openDB()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("D://OneDrive - AuroraNotebook//codeField//projects//mail//mine//manageVer//database//344.db");
        if (!db.open()) {
        qWarning() << "Error: 打开数据库失败" << db.lastError();
        return false;
    }
    return true;
}
