#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include    <QMainWindow>
#include    <QMdiSubWindow>
#include <QVector>
#include "newsocket.h"

QT_BEGIN_NAMESPACE
namespace Ui {class MainWindow;}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QString id;
    QString code;
    //QVector<NewSocket*> newSockets;
    NewSocket* curNewSocket = nullptr;

    //枚举类型treeItemType， 用于创建 QTreeWidgetItem 时作为节点的type, 自定义类型必须大于1000
    //itTopItem 顶层节点;  itGroupItem 组节点； itImageItem 图片
    //enum treeItemType{itTopItem = 1001, itGroupItem, itImageItem};
    //枚举类型，表示列号
    //enum    treeColNum{colItem=0, colItemType=1, colDate}; //目录树列的编号定义
    void iniTree();

protected:
    void closeEvent(QCloseEvent *event); //主窗口关闭时关闭所有子窗口

public:
    explicit MainWindow(NewSocket *newSocket, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actDoc_Open_triggered(); //打开文件

    void on_actDoc_New_triggered(); //新建文件

    void on_actCut_triggered();     //cut

    void on_actCopy_triggered();    //copy

    void on_actPaste_triggered();   //paste

    void on_actFont_triggered();    //设置字体

    void on_mdiArea_subWindowActivated(QMdiSubWindow *arg1);    //子窗口被激活

    void on_actViewMode_triggered(bool checked);    //MDI 模式设置

    void on_actCascade_triggered(); //级联形式显示子窗口

    void on_actTile_triggered();    //平铺形式显示子窗口

    void on_actCloseALL_triggered(); //关闭全部子窗口

    void on_actDoc_Save_triggered();

//    void on_actViewMode_triggered();

//    void on_actViewMode_triggered();

    void on_actNewAcc_triggered();

    void on_actPullMails_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // QWMAINWINDOW_H
