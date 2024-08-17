#include "newsocket.h"
#include <QThread>

void NewSocket::connectToServer()
{
    qDebug() << "成功连接到服务器";
}

void NewSocket::disconnectFromServer()
{
    qDebug() << "与服务器断开连接";
    //this->reject();
}

void NewSocket::getMessage()
{
    QString message = socket->readAll();
    if (!message.startsWith("2") && !message.startsWith("3")) {
        qWarning() << "something is wrong: ";
        flag = false;
    }

    QStringList lines = message.split("\r\n", Qt::SkipEmptyParts);

    for (const QString line : lines) {
        qDebug() << "-- 服务器：" << line;
    }
}

void NewSocket::onErrorConnection()
{
    qWarning() << "连接错误：" << socket->errorString();
    //this->reject();
}

NewSocket::NewSocket(QObject *parent) : QObject(parent) {
    qDebug() << "创建了一个 newSocket 对象";

    socket = new QTcpSocket;

    connections();
}

NewSocket::~NewSocket()
{
    qDebug() << "销毁了一个 newSocket 对象";
}

void NewSocket::sendCommand(QString command)
{
    command = command.replace("\n", "\r\n").append("\r\n");
    QByteArray msg = command.toUtf8();
    socket->write(msg);
    qDebug() << "我：" << command;
}

bool NewSocket::login(QString id, QString code)
{
    sendCommand(id.toUtf8().toBase64());
    sendCommand(code.toUtf8().toBase64());
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
    sendCommand("MAIL FROM:<" + from + ">");
    //if (!readResponse()) return false;
    if (!flag) return flag;

    sendCommand("RCPT TO:<" + to + ">");
    //if (!readResponse()) return false;
    if (!flag) return flag;

    sendCommand("DATA");
    //if (!readResponse()) return false;
    if (!flag) return flag;

    writeLetter(from, to, subject, body);

    sendCommand("QUIT");
    //if (!readResponse()) return false;
    if (!flag) return flag;

    //TODO: 需要有一个地方可以解除连接  socket->disconnectFromHost();
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
        socket->write(msg.toUtf8());
    qDebug() << msg;
    return flag;
}

void NewSocket::connections()
{
    connect(socket, SIGNAL(connected()), this, SLOT(connectToServer()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnectFromServer()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(getMessage()));
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(onErrorConnection()));
}

bool NewSocket::tryConnection(int num)
{
    socket->abort();

    socket->connectToHost("smtp.qq.com", 25);
    if (!socket->waitForConnected()) {
        qWarning() << "第 1 次连接 smtp.qq.com 服务器失败";
        for (int i = 2; i <= num; i++) {
            socket->connectToHost("smtp.qq.com", 25);
            if (!socket->waitForConnected()) {
                qWarning() << "第 " << i << " 次连接 smtp.qq.com 服务器失败";
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

    sendCommand("EHLO smtp.qq.com");
    // if (!readResponse()) {  这里会跟 getMessage 这个槽函数冲突
    //     return false;
    // }

    sendCommand("AUTH LOGIN");
    // if (!readResponse()) {
    //     return false;
    // }

    return true;
}

bool NewSocket::socketState()
{
    if (socket->isValid()) {
        switch (socket->state()) {
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
    } else {
        qDebug() << "socket is not valid";
    }
    return false;
}

QTcpSocket *NewSocket::getSocket()
{
    return socket;
}

bool NewSocket::readResponse() {
    while (!socket->waitForReadyRead(30000)) {
        qDebug() << "Waiting...";
    }
    QString response = socket->readAll();
    qDebug() << "Response:" << response;
    return response.startsWith("2") || response.startsWith("3");
}

