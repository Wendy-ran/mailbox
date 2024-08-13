#include "newdatabase.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlTableModel>

newDatabase::newDatabase() {

}

newDatabase::~newDatabase()
{

}

bool newDatabase::openDB(QString dbFilePath)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    // "D://OneDrive - AuroraNotebook//codeField//projects//mail//mine//database//user1.db"
    db.setDatabaseName(dbFilePath);
        if (!db.open()) {
        qWarning() << "Error: 打开数据库失败" << db.lastError();
        return false;
    }
    return true;
}

void newDatabase::setupModelAndView(QTreeView *view, QString tableName) {
    QSqlTableModel *model = new QSqlTableModel();
    //model->setTable("draft");  // 设置模型使用的表
    model->setTable(tableName);
    model->select();  // 加载数据

    // 设置表头
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Datetime"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Sender"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Receiver"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Subject"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Body"));

    view->setModel(model);  // 将模型设置到 QTreeView
}



