#include "mainwindow.h"

#include <QApplication>
#include    "tdialogLogin.h"
#include <QTcpSocket>
#include "newsocket.h"
#include <QMessageBox>
#include "objectpool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 使用单例模式的 ObjectPool
    auto& pool = ObjectPool<NewSocket>::getInstance();
    NewSocket* newSocket1 = pool.acquire("originUser");
    //NewSocket *newSocket = new NewSocket;

    bool conFlag = newSocket1->tryConnection(5);
    TDialogLogin *dlgLogin = new TDialogLogin(newSocket1);  //创建Splash对话框
    int ret = dlgLogin->exec();

    if (ret == QDialog::Accepted && conFlag) {
        MainWindow w(newSocket1);       //显示主窗口
        w.show();
        return a.exec();    //应用程序正常运行
    } else if (!conFlag) {
        QMessageBox::critical(nullptr, "连接错误", "无法连接 QQ 服务器");
        return 0;
    } else {
        QMessageBox::critical(nullptr, "错误", "程序初始化失败");
        return 0;
    }
}

// QApplication a(argc, argv);
// MainWindow w;
// w.show();
// return a.exec();
