#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSslSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // QSslSocket *socket = new QSslSocket;
    // socket->connectToHostEncrypted("imap.qq.com", 993);\
    // qDebug() << "连接服务器...";
    // if (socket->waitForBytesWritten()) {
    //     qDebug() << socket->readAll();
    // }

    // QString commend = "A1 LOGIN 3445003795@qq.com lddvdbkivnapchcb\r\n";
    // socket->write(commend.toUtf8());
    // qDebug() << commend;
    // if (socket->waitForReadyRead()) {
    //     qDebug() << socket->readAll();
    // }

    // commend = "a1 LIST \"\" \"*\"\r\n";
    // socket->write(commend.toUtf8());
    // qDebug() << commend;
    // if (socket->waitForReadyRead()) {
    //     qDebug() << socket->readAll();
    // }

    // qDebug() << socket->readAll();

    QSslSocket *socket = new QSslSocket;
    socket->connectToHostEncrypted("imap.qq.com", 993);
    qDebug() << "连接服务器...";
    // 等待连接成功
    if (!socket->waitForConnected()) {
        qDebug() << "连接失败:" << socket->errorString();
        return;
    }
    if (socket->waitForReadyRead()) {
        qDebug() << socket->readAll();
    }

    QString command = "A1 LOGIN 3445003795@qq.com lddvdbkivnapchcb\r\n";
    socket->write(command.toUtf8());
    qDebug() << command;
    if (socket->waitForReadyRead()) {
        qDebug() << socket->readAll();
    }

    command = "a1 LIST \"\" \"*\"\r\n";
    socket->write(command.toUtf8());
    qDebug() << command;
    if (socket->waitForReadyRead()) {
        qDebug() << socket->readAll();
    }

    socket->waitForReadyRead();
    qDebug() << socket->readAll();

}

MainWindow::~MainWindow()
{
    delete ui;
}
