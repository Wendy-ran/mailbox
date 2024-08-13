#include "mainwindow.h"

#include <QApplication>
#include    "tdialogLogin.h"
#include <QTcpSocket>
#include "newsocket.h"
#include <QMessageBox>
#include "objectpool.h"
#include <QSslSocket>
#include <QNetworkAccessManager>
#include <QSslSocket>

// #include <cryptlib.h>
// #include <osrng.h>
// #include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug()<< "ssl: " << QSslSocket::sslLibraryBuildVersionString();
    // 指定 OpenSSL 库的路径
    // qputenv("OPENSSL_CONF", "D://apps//OpenSSL-Win64//bin//cnf//openssl.cnf");
    // qputenv("SSL_CERT_FILE", "D://apps//OpenSSL-Win64//bin//PEM//cert.pem");

    qDebug() << "SSL library Build Version String:" << QSslSocket::sslLibraryBuildVersionString();


    //看看是否配置了ssl
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    qDebug() << manager->supportedSchemes();
    qDebug() << "-----------------------------------------";

    // try {
    //     CryptoPP::AutoSeededRandomPool rng;
    //     CryptoPP::byte key[16]; // 128-bit key
    //     rng.GenerateBlock(key, sizeof(key));

    //     std::cout << "Generated key: ";
    //     for (int i = 0; i < sizeof(key); ++i) {
    //         std::cout << std::hex << (int)key[i];
    //     }
    //     std::cout << std::endl;
    // } catch (const CryptoPP::Exception& e) {
    //     std::cerr << "Crypto++ error: " << e.what() << std::endl;
    //     //return 1;
    // }

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
        //QMessageBox::critical(nullptr, "错误", "程序初始化失败");
        return 0;
    }
}

// QApplication a(argc, argv);
// MainWindow w;
// w.show();
// return a.exec();
