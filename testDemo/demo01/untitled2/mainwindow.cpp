#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imapclient.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ImapClient client("imap.qq.com", 993); // Use SSL port for encrypted connection
    client.connectToServer();
    client.login("3445003795@qq.com", "lddvdbkivnapchcb");
    client.selectInbox();
    client.fetchEmailBody(10); // Fetch body of the 10th email
}

MainWindow::~MainWindow()
{
    delete ui;
}
