#include "newsocket.h"
#include <QThread>
#include <QSslError>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include "utils.h"
//#include "mainwindow.h"


void NewSocket::connections()
{
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(connectToServer()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnectFromServer()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(getMessage()));
    connect(tcpSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(onErrorConnection()));

    connect(sslSocket, &QSslSocket::connected, this, &NewSocket::connectToServer);
    connect(sslSocket, &QSslSocket::encrypted, this, &NewSocket::onEncrypted);
    //connect(sslSocket, &QSslSocket::readyRead, this, &NewSocket::getMessage);
    connect(sslSocket, &QSslSocket::disconnected, this, &NewSocket::disconnectFromServer);
    connect(sslSocket, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslErrors(const QList<QSslError>&)));
    connect(this, SIGNAL(gotBoxNames(QString)), this, SLOT(onBoxNames(QString)));
}

void NewSocket::connectToServer()
{
    QObject *obj = sender();
    if (obj == tcpSocket) {
        qDebug() << "tcpSocket 成功连接到服务器";
    } else if (obj == sslSocket) {
        qDebug() << "sslSocket 成功连接到服务器";
    } else {
        qDebug() << "unkown sender";
    }
}

void NewSocket::disconnectFromServer()
{
    QObject *obj = sender();
    if (obj == tcpSocket) {
        qDebug() << "tcpSocket 与服务器断开连接";
    } else if (obj == sslSocket) {
        qDebug() << "sslSocket 与服务器断开连接";
    } else {
        qDebug() << "unkown sender";
    }
}

void NewSocket::getMessage()
{
    QObject *obj = sender();
    QString message = "";
    if (obj == tcpSocket) {
        message = tcpSocket->readAll();
        if (!message.startsWith("2") && !message.startsWith("3")) {
            qWarning() << "newsocket: tcpSocket has something wrong!";
            flag = false;
        }
    } else if (obj == sslSocket) {
        message = sslSocket->readAll();

        // if (message.contains("* LIST ")) {  // 文件夹列表
        //     emit gotBoxNames(message);
        // }
    } else {
        qDebug() << "unkown sender";
        return;
    }

    QStringList lines = message.split("\r\n", Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        qDebug() << "-- 服务器：" << line;
    }
}

void NewSocket::onErrorConnection()
{
    qWarning() << "newsocket: 连接错误：" << tcpSocket->errorString();
}

void NewSocket::onEncrypted()
{
    qDebug() << "Encrypted connection established";
}

void NewSocket::onSslErrors(const QList<QSslError> &msgs)
{
    qWarning() << "newsocket: sslSocket is something wrong";
    for (const QSslError &msg : msgs) {
        qDebug() << "SSL Error:" << msg.errorString();
    }
}

// void NewSocket::onBoxNames(QString names)
// {
//     QStringList boxNames = names.split("\r\n", Qt::SkipEmptyParts), res;

//     //QList<QString> folderNames;
//     QRegularExpression regex(R"(\* LIST.*\"/\" \"(.*?)\")");  // 正则表达式匹配包含文件夹名的模式

//     for (const QString &boxName : boxNames) {
//         QRegularExpressionMatch match = regex.match(boxName);
//         if (match.hasMatch()) {
//             QString name = match.captured(1); // 提取第一个捕获组，即文件夹名
//             qDebug() << "Box:" << name;

//             if (!Utils::containsNonEnglish(name)) {
//                 name = matchBoxName(name);
//                 res.append(name);
//             }
//         }
//     }

//     emit transferBoxNames(res);
// }

NewSocket::NewSocket(QObject *parent) : QObject(parent) {
    qDebug() << "创建了一个 newSocket 对象";

    tcpSocket = new QTcpSocket;
    sslSocket = new QSslSocket;

    connections();
}

NewSocket::~NewSocket()
{
    qDebug() << "销毁了一个 newSocket 对象";
}

void NewSocket::sendCommand(QString command, QString serverName)
{
    command = command.replace("\n", "\r\n").append("\r\n");
    QByteArray msg = command.toUtf8();

    if (serverName == "smtp.qq.com") {
        tcpSocket->write(msg);
    } else if (serverName == "imap.qq.com") {
        sslSocket->write(msg);
    } else {
        qWarning() << "newsocket: serverName 不明：serverName 为 " << serverName;
        return;
    }
    qDebug() << "我向 " << serverName << " 服务器发送命令 " << command;
}

bool NewSocket::login(QString id, QString code)
{
    sendCommand(id.toUtf8().toBase64(), "smtp.qq.com");
    sendCommand(code.toUtf8().toBase64(), "smtp.qq.com");
    this->id = id;
    this->code = code;
    return flag;
}

QString NewSocket::getId()
{
    return id;
}

QString NewSocket::getCode()
{
    return code;
}

bool NewSocket::sendEmail(const QString &from, const QString &to, const QString &subject, const QString &body)
{
    sendCommand("MAIL FROM:<" + from + ">", "smtp.qq.com");
    //if (!readResponse()) return false;
    if (!flag) return flag;

    sendCommand("RCPT TO:<" + to + ">", "smtp.qq.com");
    //if (!readResponse()) return false;
    if (!flag) return flag;

    sendCommand("DATA", "smtp.qq.com");
    //if (!readResponse()) return false;
    if (!flag) return flag;

    writeLetter(from, to, subject, body);

    sendCommand("QUIT", "smtp.qq.com");
    //if (!readResponse()) return false;
    if (!flag) return flag;

    // TODO: 需要有一个地方可以解除连接  socket->disconnectFromHost();
    return true;
}

bool NewSocket::writeLetter(const QString &from, const QString &to, const QString &subject, const QString &body)
{
    QString msg = "From: " + from + "\r\n";
    msg += "To: " + to + "\r\n";
    msg += "Subject: " + subject + "\r\n";

    // 纯文本邮件
    msg.append("MIME-Version: 1.0\r\n")
        .append("Content-Type: multipart/mixed; boundary=\"boundary42\"\r\n\r\n")
        .append("\r\n")
        .append(body + "\r\n.\r\n")
        .append("QUIT\r\n");
        tcpSocket->write(msg.toUtf8());
    qDebug() << msg;
    return flag;
}

QStringList NewSocket::getBoxNames()
{
    //QStringList list;
    // 列举出所有的文件夹
    QString str = Utils::readLineFromFile(":/config/something.txt", 1);  //a1 LIST "" "*"
    sendCommand(str, "imap.qq.com");
    if (sslSocket->waitForReadyRead()) {
        QString message = sslSocket->readAll();
        qDebug() << "-- 服务器：" << message;
        return Utils::extractBoxNames(message);

        // QStringList lines = message.split("\r\n", Qt::SkipEmptyParts);
        // for (const QString &line: lines) {

        //     qDebug() << "-- 服务器：" << line;
        // }
    } else {
        qWarning() << "newsocket: 没有读到 imap 回复";
        //return false;
    }
    //return list;
    //return true;
}

bool NewSocket::getHisLetters()
{
    return true;
}

bool NewSocket::tryConnection(QString server, int num)
{
    // 判断要连哪个服务器
    if (server == "smtp.qq.com") {
        tcpSocket->abort();

        tcpSocket->connectToHost("smtp.qq.com", 25);
        if (!tcpSocket->waitForConnected()) {
            qWarning() << "newsocket: 第 1 次连接 smtp.qq.com 服务器失败";
            for (int i = 2; i <= num; i++) {
                tcpSocket->connectToHost("smtp.qq.com", 25);
                if (!tcpSocket->waitForConnected()) {
                    qWarning() << "newsocket: 第 " << i << " 次连接 smtp.qq.com 服务器失败";
                } else {
                    qDebug() << "连接 smtp.qq.com 服务器成功";
                    break;
                }
                if (i == num) {
                    return false;
                }
            }
        } else {
            qDebug() << "连接 smtp.qq.com 服务器成功";
        }

        sendCommand("EHLO smtp.qq.com", "smtp.qq.com");
        // if (!readResponse()) {  这里会跟 getMessage 这个槽函数冲突
        //     return false;
        // }

        sendCommand("AUTH LOGIN", "smtp.qq.com");
        // if (!readResponse()) {
        //     return false;
        // }
    } else if (server == "imap.qq.com") {
        sslSocket->abort();

        sslSocket->connectToHostEncrypted("imap.qq.com", 993);
        if (sslSocket->waitForConnected(30000)) {
            if (sslSocket->waitForReadyRead()) {
                qDebug() << "-- 服务器：" << sslSocket->readAll();
            }
        } else {
            qWarning() << "newsocket: 无法连接 imap.qq.com 服务器，" << sslSocket->errorString();
            return false;
        }

        if (id != "" && code != "") {
            sendCommand("A1 LOGIN " + id + " " + code, "imap.qq.com");
            if (sslSocket->waitForReadyRead()) {
                qDebug() << "-- 服务器：" << sslSocket->readAll();
            } else {
                qWarning() << "sslSocket 登陆失败";
                return false;
            }
        } else {
            qWarning() << "newsocket: id 和 code 未赋值";
            return false;
        }
    } else {
        qWarning() << "newsocket: server 名称不可用";
        return false;
    }
    return true;
}

bool NewSocket::socketState(QString socketName)
{
    enum QAbstractSocket::SocketState state;
    if (socketName == "tcp") {
        if (tcpSocket->isValid()) {
            state = tcpSocket->state();
        } else {
            qWarning() << "newsocket: tcpSocket is not valid";
            return false;
        }
    } else if (socketName == "ssl") {
        if (sslSocket->isValid()) {
            state = sslSocket->state();
        } else {
            qWarning() << "newsocket: sslSocket is not valid";
            return false;
        }
    } else {
        qWarning() << "newsocket: socketName is wrong";
        return false;
    }
    switch (state) {
        case QAbstractSocket::UnconnectedState:
            qDebug() << "socket state is not connected";
            break;
        case QAbstractSocket::HostLookupState:
            qDebug() << "The socket is performing a host name lookup";
            break;
        case QAbstractSocket::ConnectingState:
            qDebug() << "The socket has started establishing a connection";
            break;
        case QAbstractSocket::ConnectedState:
            qDebug() << "A connection is established";
            return true;
            break;
        case QAbstractSocket::BoundState:
            qDebug() << "The socket is bound to an address and port";
            break;
        case QAbstractSocket::ClosingState:
            qDebug() << "The socket is about to close (data may still be waiting to be written)";
            break;
        case QAbstractSocket::ListeningState:
            qDebug() << "For internal use only";
            break;
        default:
            break;
        }
    return false;
}

QTcpSocket *NewSocket::getTcpSocket()
{
    return tcpSocket;
}

QSslSocket *NewSocket::getSslSocket()
{
    return sslSocket;
}

// bool NewSocket::readResponse() {
//     while (!tcpSocket->waitForReadyRead(30000)) {
//         qDebug() << "Waiting...";
//     }
//     QString response = tcpSocket->readAll();
//     qDebug() << "Response:" << response;
//     return response.startsWith("2") || response.startsWith("3");
// }

