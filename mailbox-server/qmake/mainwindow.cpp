#include "mainwindow.h"
#include "ui_mainwindow.h"
#include    <QtNetwork>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>

void MainWindow::connections()
{
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(do_newConnection()));
    connect(this, SIGNAL(stackNumChanged()), this, SLOT(onStackNumChanged()));
    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
}

void MainWindow::initUI()
{
    this->setWindowTitle("Server");

    // 状态栏
    labListen = new QLabel("监听状态:");
    labListen->setMinimumWidth(150);
    ui->statusBar->addWidget(labListen);
    labSocketState = new QLabel("Socket状态：");
    labSocketState->setMinimumWidth(200);
    ui->statusBar->addWidget(labSocketState);

    // 标题栏
    QString localIP = getLocalIP();   //获取本机IP
    this->setWindowTitle(this->windowTitle() + " - " + localIP);
    ui->comboIP->addItem(localIP);

    // 几个按钮
    QString btnStyle = "QPushButton {"
                       "   background-color: #f0f0f0;"
                       "   border: 1px solid #c0c0c0;"
                       "   border-radius: 4px;"
                       "   padding: 5px;"
                       "   color: black;"
                       "}"
                       "QPushButton:hover {"
                       "   background-color: #e1e1e1;"
                       "}"
                       "QPushButton:pressed {"
                       "   background-color: #cccccc;"
                       "}"
                       "QPushButton:disabled {"
                       "   background-color: #e0e0e0;"
                       "   color: #a0a0a0;"
                       "   border: 1px solid #d0d0d0;"
                       "}";

    ui->btnPrevP->setStyleSheet(btnStyle);
    ui->btnPrevP->setEnabled(false);
    ui->btnNextP->setStyleSheet(btnStyle);
    ui->btnNextP->setEnabled(false);

    ui->btnClear->setStyleSheet(btnStyle);
    ui->btnCmds->setStyleSheet(btnStyle);
    ui->btnNewP->setStyleSheet(btnStyle);
    ui->btnSave->setStyleSheet(btnStyle);
    ui->btnSend->setStyleSheet(btnStyle);

    //ui->actStart->setEnabled(false);
    ui->actStop->setEnabled(false);
    ui->btnSend->setEnabled(false);

    // 获取页面的 QWidget 指针
    QWidget* widgetToRemove = ui->stackedWidget->widget(1); // 获取索引为1的页面
    if (widgetToRemove) {
        ui->stackedWidget->removeWidget(widgetToRemove); // 从 QStackedWidget 中移除该页面
        delete widgetToRemove; // 删除页面，释放资源
    }
    widgetToRemove = ui->stackedWidget->widget(0); // 获取索引为1的页面
    if (widgetToRemove) {
        ui->stackedWidget->removeWidget(widgetToRemove); // 从 QStackedWidget 中移除该页面
        delete widgetToRemove; // 删除页面，释放资源
    }
    on_btnNewP_clicked();
    ui->labPageNum->setText(QString::number(ui->stackedWidget->currentIndex()+1) + "/" + QString::number(ui->stackedWidget->count()));
    // textEdits.append(ui->textEdit);
    // curWriteEdit = ui->textEdit;
}

void MainWindow::appendText(QTextEdit* textEdit, QString text, QString color)
{
    //textEdit->append("<font style='color: rgb(49, 101, 95);'>这是蓝色文本</font>");
    textEdit->append("<font style='color: " + color + ";'>" + text + "</font>");
}

QString MainWindow::getLocalIP()  //获取本机IPv4地址
{
    QString hostName=QHostInfo::localHostName();    //本地主机名
    QHostInfo hostInfo=QHostInfo::fromName(hostName);
    QString   localIP="";

    QList<QHostAddress> addList=hostInfo.addresses();  //本机IP地址列表
    if (addList.isEmpty())
        return localIP;

    foreach(QHostAddress aHost, addList)
        if (QAbstractSocket::IPv4Protocol==aHost.protocol())
        {
            localIP=aHost.toString();
            break;
        }
    return localIP;
}

//void MainWindow::closeEvent(QCloseEvent *event)
//{//关闭窗口时停止监听
//    if (tcpServer->isListening())
//        tcpServer->close();;//停止网络监听
//    event->accept();
//}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initUI();

    tcpServer=new QTcpServer(this);
    connections();

    // 默认工作目录
    workDir = "D:\\codeField\\mailbox\\files";
}

MainWindow::~MainWindow()
{//析构函数
    if (tcpSocket != nullptr)
    {
        if (tcpSocket->state()==QAbstractSocket::ConnectedState)
            tcpSocket->disconnectFromHost();    //断开与客户端的连接
    }

    if (tcpServer->isListening())
        tcpServer->close();     //停止网络监听

    delete ui;
}

void MainWindow::do_newConnection()
{
    tcpSocket = tcpServer->nextPendingConnection(); //创建socket

    connect(tcpSocket, SIGNAL(connected()),this, SLOT(do_clientConnected()));
    do_clientConnected();   //执行一次槽函数，显示状态

    connect(tcpSocket, SIGNAL(disconnected()),this, SLOT(do_clientDisconnected()));

    connect(tcpSocket,&QTcpSocket::stateChanged,this,&MainWindow::do_socketStateChange);
    do_socketStateChange(tcpSocket->state());   //执行一次槽函数，显示状态

    connect(tcpSocket,SIGNAL(readyRead()),  this,SLOT(do_socketReadyRead()));
}

void MainWindow::do_socketStateChange(QAbstractSocket::SocketState socketState)
{//socket状态变化时
    switch(socketState)
    {
    case QAbstractSocket::UnconnectedState:
        labSocketState->setText("socket状态：UnconnectedState");
        break;
    case QAbstractSocket::HostLookupState:
        labSocketState->setText("socket状态：HostLookupState");
        break;
    case QAbstractSocket::ConnectingState:
        labSocketState->setText("socket状态：ConnectingState");
        break;
    case QAbstractSocket::ConnectedState:
        labSocketState->setText("socket状态：ConnectedState");
        break;
    case QAbstractSocket::BoundState:
        labSocketState->setText("socket状态：BoundState");
        break;
    case QAbstractSocket::ClosingState:
        labSocketState->setText("socket状态：ClosingState");
        break;
    case QAbstractSocket::ListeningState:
        labSocketState->setText("socket状态：ListeningState");
    }
}

void MainWindow::do_clientConnected()  // 客户端接入时
{
    ui->btnSend->setEnabled(true);

    //curWritePage->append("客户端连接成功！");
    appendText(curWriteEdit, "客户端连接成功！", purple);
    // curWritePage->append("客户端地址："+ tcpSocket->peerAddress().toString()
    //                      + ":" + QString::number(tcpSocket->peerPort()));
    appendText(curWriteEdit, "客户端地址："+ tcpSocket->peerAddress().toString()
                                 + ":" + QString::number(tcpSocket->peerPort()), purple);
    //curWritePage->append("客户端端口号："+ QString::number(tcpSocket->peerPort()));
}

void MainWindow::do_clientDisconnected()  // 客户端断开连接时
{
    //curWritePage->append("**client socket disconnected");
    appendText(curWriteEdit, "客户端断开连接", purple);
    tcpSocket->deleteLater();
}

void MainWindow::do_socketReadyRead() // 读取缓冲区的行文本
{
    qDebug() << "开始读...";

    while(tcpSocket->canReadLine()) {
        QString str = tcpSocket->readLine();
        //curWritePage->append("[in] "+ str);
        appendText(curWriteEdit, "[in] "+ str);
        qDebug() << str;

        if (str.trimmed() == "EHLO smtp.mine.com") {
            loginStep = 1;
            tcpSocket->write("250-smtp.qq.com\r\n250-PIPELINING\r\n250-SIZE 73400320\r\n"
                             "250-STARTTLS\r\n250-AUTH LOGIN PLAIN\r\n250-AUTH=LOGIN\r\n250-MAILCOMPRESS\r\n250 8BITMIME");
        } else if (loginStep == 2 && str.trimmed() == "AUTH LOGIN") {
            loginStep++;
            tcpSocket->write("334 VXNlcm5hbWU6");
        } else if (loginStep == 3 && str.trimmed() == "NzcxNzc4MDg3QHFxLmNvbQ==") {  // id
            loginStep++;
            tcpSocket->write("334 UGFzc3dvcmQ6");
        } else if (loginStep == 4 && str.trimmed() == "aaa") {  // id
            loginStep++;
            tcpSocket->write("235 Authentication successful");
        } else if (loginStep == 5 && str.trimmed().left(9) == "MAIL FROM") {  // 来自
            loginStep++;
            tcpSocket->write("250 Ok");
        } else if (loginStep == 6 && str.trimmed().left(9) == "RCPT TO") {  // 发给
            loginStep++;
            // TODO
            tcpSocket->write("250 Ok");
        } else if (loginStep == 7 && str.trimmed() == "DATA") {
            loginStep++;
            tcpSocket->write("354 End data with <CR><LF>.<CR><LF>");
        } else if (loginStep == 8) {  // 解析正文
            loginStep++;
            // TODO
            tcpSocket->write("250 Ok: queued as");
        }

        if (str.trimmed() == "QUIT") {
            tcpSocket->write("221 Bye");
            tcpSocket->disconnect();
        }
    }

}

void MainWindow::on_actStart_triggered()  //开始监听
{
    QString IP = ui->comboIP->currentText();  //IP地址字符串,如"127.0.0.1"
    quint16 port = ui->spinPort->value();     //端口
    QHostAddress address(IP);
    tcpServer->listen(address,port);    //开始监听
    //    tcpServer->listen(QHostAddress::LocalHost,port);// Equivalent to QHostAddress("127.0.0.1").
    // ui->textEdit->append("<font style='color: rgb(49, 101, 95);'>这是蓝色文本</font>");
    //curWritePage->append("<font style='color: rgb(49, 101, 95);'>开始监听...</font>");
    appendText(curWriteEdit, "开始监听...", purple);
    // curWritePage->append("服务器地址：" + tcpServer->serverAddress().toString()
    //                      + ":" + QString::number(tcpServer->serverPort()));
    appendText(curWriteEdit, "服务器地址：" + tcpServer->serverAddress().toString()
                                 + ":" + QString::number(tcpServer->serverPort()), purple);
    //curWritePage->append("服务器端口：" + QString::number(tcpServer->serverPort()));

    ui->actStart->setEnabled(false);
    ui->actStop->setEnabled(true);
    ui->btnSend->setEnabled(true);
    labListen->setText("监听状态：正在监听");
}

void MainWindow::on_actStop_triggered()  // 停止监听
{
    if (tcpServer->isListening())   //tcpServer正在监听
    {
        if (tcpSocket != nullptr)
            if (tcpSocket->state()==QAbstractSocket::ConnectedState)
                tcpSocket->disconnectFromHost();
        tcpServer->close();         //停止监听
        ui->actStart->setEnabled(true);
        ui->actStop->setEnabled(false);
        labListen->setText("监听状态：已停止监听");
    }
}

// void MainWindow::on_actClear_triggered()
// {
//     curWritePage->clear();
// }

void MainWindow::on_btnSend_clicked()  // "发送消息"按钮，发送一行字符串，以换行符结束
{
    QString msg = ui->editMsg->text();
    if (msg == "（导入文件内容为空）") {
        return;
    } else if (msg.right(3) == "...") {
        appendText(curWriteEdit, "[out] " + commands);
        QByteArray str = commands.toUtf8();
        str.append('\n');       //添加一个换行符
        tcpSocket->write(str);
    } else {
        appendText(curWriteEdit, "[out] " + msg);
        QByteArray str = msg.toUtf8();
        str.append('\n');       //添加一个换行符
        tcpSocket->write(str);
    }

    ui->editMsg->clear();
    ui->editMsg->setFocus();
}

void MainWindow::on_actHostInfo_triggered()
{//获取本机地址, 用于测试
    QString hostName=QHostInfo::localHostName();//本地主机名
    //curWritePage->append("本机主机名："+hostName+"\n");
    appendText(curWriteEdit, "本机主机名："+hostName+"\n");
    QHostInfo   hostInfo=QHostInfo::fromName(hostName);
    QList<QHostAddress> addList=hostInfo.addresses();
    if (addList.isEmpty())
        return;

    foreach (QHostAddress aHost, addList)
        if (QAbstractSocket::IPv4Protocol==aHost.protocol())
        {
            QString IP=aHost.toString();
            //curWritePage->append("本机IP地址："+aHost.toString());
            appendText(curWriteEdit, "本机IP地址："+aHost.toString());
            if (ui->comboIP->findText(IP)<0)
                ui->comboIP->addItem(IP);
        }
}


void MainWindow::on_btnClear_clicked()
{
    textEdits[ui->stackedWidget->currentIndex()]->clear();
}


void MainWindow::on_btnNewP_clicked()
{
    // 创建新页面
    QWidget *newPage = new QWidget();

    // 创建一个 QTextEdit
    QTextEdit *textEdit = new QTextEdit();
    // textEdit->setPlainText("这里可以输入文本");  // 可以设置默认文本或保留为空

    // 创建布局并将 QTextEdit 添加到布局中
    QVBoxLayout *layout = new QVBoxLayout(newPage);
    layout->addWidget(textEdit);

    // 设置布局的边距为0，确保 QTextEdit 充满整个页面
    layout->setContentsMargins(0, 0, 0, 0);

    // 设置布局
    newPage->setLayout(layout);

    // 将新页面添加到 stackedWidget
    ui->stackedWidget->addWidget(newPage);

    // 可选：切换到新页面
    ui->stackedWidget->setCurrentWidget(newPage);
    curWriteEdit = textEdit;
    textEdits.append(textEdit);

    emit stackNumChanged();
}


void MainWindow::on_btnPrevP_clicked()
{
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()-1);
}


void MainWindow::on_btnNextP_clicked()
{
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()+1);
}


void MainWindow::on_actChoseField_triggered() // 选择工作目录
{
    workDir = QFileDialog::getExistingDirectory(this, "选择一个文件夹", QDir::rootPath(), QFileDialog::ShowDirsOnly);
    //ui->actStart->setEnabled(true);
}

void MainWindow::onStackNumChanged()
{
    ui->labPageNum->setText(QString::number(ui->stackedWidget->currentIndex()+1) + "/" + QString::number(ui->stackedWidget->count()));
}

void MainWindow::onCurrentChanged(int index)
{
    if (index == 0) {
        ui->btnPrevP->setEnabled(false);
        if (ui->stackedWidget->count() > 1) {
            ui->btnNextP->setEnabled(true);
        } else {
            ui->btnNextP->setEnabled(false);
        }
    } else if (index == ui->stackedWidget->count()-1) {
        ui->btnPrevP->setEnabled(true);
        ui->btnNextP->setEnabled(false);
    } else {
        ui->btnNextP->setEnabled(true);
        ui->btnPrevP->setEnabled(true);
    }

    ui->labPageNum->setText(QString::number(ui->stackedWidget->currentIndex()+1) + "/" + QString::number(ui->stackedWidget->count()));
}


void MainWindow::on_btnSave_clicked()
{
    if (!saveFlag) {
        saveFlag = true;
        createSubfolder(workDir, "log");
    }
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString title = currentDateTime.toString("yyyyMMddHHmmss") + ".txt";  // curWriteEdit->toPlainText().left(10);
    createTextFile(workDir + QDir::separator() + "log", title, curWriteEdit->toPlainText());
}

bool MainWindow::createSubfolder(const QString &workDir, const QString &subDir) {
    QDir dir(workDir);

    // 检查目录是否存在，如果不存在，则先创建
    if (!dir.exists()) {
        if (!dir.mkpath(".")) { // 尝试创建工作目录
            qWarning() << "Failed to create the work directory:" << workDir;
            return false;
        }
    }

    // 创建子文件夹 'log'
    if (dir.mkdir(subDir)) {
        qDebug() << "Log directory created successfully.";
        return true;
    } else {
        if (dir.exists(subDir)) {
            qDebug() << "Log directory already exists.";
        } else {
            qDebug() << "Failed to create log directory.";
        }
        return false;
    }
}

bool MainWindow::createTextFile(const QString &dir, const QString &file, const QString &text) {
    // 确保目录存在，如果不存在则创建
    QDir directory(dir);
    if (!directory.exists()) {
        if (!directory.mkpath(".")) {  // 尝试在当前的 QDir 对象所指向的路径创建目录
            qWarning() << "Error: Unable to create directory" << dir;
            return false;
        }
    }

    // 构造完整的文件路径
    QString filePath = QDir::cleanPath(dir + QDir::separator() + file);

    // 创建并打开文件
    QFile outputFile(filePath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: Unable to open file for writing:" << filePath;
        return false;
    }

    // 创建文本流并写入文本
    QTextStream out(&outputFile);
    out << text;

    // 关闭文件
    outputFile.close();
    qDebug() << "File written successfully:" << filePath;

    return true;
}

void MainWindow::on_btnCmds_clicked()  // 导入命令
{
    // 打开文件选择对话框
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Text File"),
        "",
        tr("Text Files (*.txt)")
        );

    // 检查用户是否真的选择了文件
    if (filePath.isEmpty()) {
        qDebug() << "No file selected.";
        return;
    }

    // 调用读取文件的函数
    commands = readTextFile(filePath);
    qDebug() << "File content:" << commands;

    if (commands.isEmpty()) {
        ui->editMsg->setText("（导入文件内容为空）");
    } else {
        ui->editMsg->setText(commands.left(35) + "...");
    }
}

QString MainWindow::readTextFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open the file for reading.";
        return QString(); // 返回一个空的 QString
    }

    QTextStream stream(&file);
    QString content = stream.readAll(); // 读取文件的所有内容
    file.close(); // 关闭文件

    return content; // 返回读取的内容
}
