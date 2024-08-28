#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    socket = new QSslSocket;

    connect(socket, SIGNAL(connected()), this, SLOT(connectToServer()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnectFromServer()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(getMessage()));
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(onErrorConnection()));
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_btn_send_clicked()
{
    // socket->abort();
    // socket->connectToHostEncrypted("imap.qq.com", 993);
    // QString str = "A1 LOGIN 3445003795@qq.com lddvdbkivnapchcb\r\n";
    // socket->write(str.toUtf8());
    // str = "2 SELECT INBOX\r\n";
    // socket->write(str.toUtf8());
    // ui->msgs->append("===========================");

    QString plainText = ui->sender->toPlainText();
    plainText.replace("\n", "\r\n").append("\r\n");
    //QByteArray message = QByteArray().append(plainText);
    socket->write(plainText.toUtf8());

    plainText.replace("<", "&lt;").replace(">", "&gt;");
    QString info = QString("<font style='color:blue'>我：<br>%1</font>").arg(plainText);
    ui->msgs->append(info.replace("\r", "").replace("\n", "<br>"));
    ui->sender->clear();
}


void MainWindow::on_btn_connect_clicked()
{
    socket->abort();
    socket->connectToHostEncrypted("imap.qq.com", 993);

    //-------------
    QString str = "A1 LOGIN 3445003795@qq.com lddvdbkivnapchcb\r\n";
    socket->write(str.toUtf8());
    str = "2 SELECT INBOX\r\n";
    socket->write(str.toUtf8());
}

void MainWindow::connectToServer()
{
    qDebug() << "成功连接到服务器";
}

void MainWindow::disconnectFromServer()
{
    qDebug() << "与服务器断开连接";
    //this->reject();
}

void MainWindow::getMessage()
{
    QString message = socket->readAll();
    appendMessageWithColor("-- 服务器：" + message, "red");

    // if (!message.startsWith("2") && !message.startsWith("3")) {
    //     qWarning() << "something is wrong: ";
    //     //flag = false;
    // }

    //QStringList lines = message.split("\r\n\r\n", Qt::SkipEmptyParts);

    // for (const QString &line : lines) {
    //     qDebug() << "-- 服务器：" << line;
    //     ui->msgs->append("服务器：" + line);
    // }

    //ui->msgs->append("服务器：" + message);
}

void MainWindow::onErrorConnection()
{
    qWarning() << "连接错误：" << socket->errorString();
    //this->reject();
}

void MainWindow::appendMessageWithColor(const QString &message, const QString &color) {
    QString coloredMessage = QString("<span style='color: %1;'>%2</span>").arg(color).arg(message);
    ui->msgs->append(coloredMessage);
}
