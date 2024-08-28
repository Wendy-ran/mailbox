#include "newsocket.h"
#include <QThread>
#include <QSslError>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QString>
#include "utils.h"
#include <QStringDecoder>
#include <algorithm>

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
    connect(sslSocket, SIGNAL(sslErrors(const QList<QSslError>)), this, SLOT(onSslErrors(const QList<QSslError>&)));
    //connect(this, SIGNAL(gotBoxNames(QString)), this, SLOT(onBoxNames(QString)));
}

bool NewSocket::getMailOfBox(EmailInfo &mail, QString boxName, int id)
{
    if (!(boxName.at(0).isLetter() && boxName.at(0).unicode() <= 127)) {
        boxName = Utils::matchBoxName(boxName);
    }

    if (boxName == "INBOX") {  // 收件箱
        sendCommand("2 SELECT INBOX", "imap.qq.com");
        QString msg = "";
        if (sslSocket->waitForReadyRead()) {
            msg = sslSocket->readAll();
            qDebug() << "-- 服务器：" << msg;
        } else {
            qWarning() << "2 SELECT INBOX 命令服务器返回失败";
            return false;
        }

        // 提取“总邮件数”
        int total = 0;
        QRegularExpression regex(R"(\* (\d+) EXISTS)");
        match = regex.match(msg);
        if (match.hasMatch()) {
            total = match.captured(1).toInt();  // 将提取的数字转换为整型
        }
        if (total == 0) {
            qWarning() << boxName << " 中没有邮件";
            return false;
        } else if (id > total) {
            qWarning() << "id 数超过了 " << boxName << " 邮箱中的邮件总数了";
            return false;
        }

        // 100 FETCH xx BODY[header.fields (Date From Subject)]  单条邮件
        sendCommand("a0 FETCH " + QString::number(id) + " BODY[header.fields (Date From Subject)]", "imap.qq.com");
        if (sslSocket->waitForReadyRead()) {
            msg = sslSocket->readAll();
            qDebug() << "-- 服务器：" << msg;
        } else {
            qWarning() << "100 FETCH xx BODY[header.fields (Date From Subject)] 命令服务器返回失败";
            return false;
        }

        // 处理服务器返回数据 这里只有单条
        if (!msg.contains("a0 OK FETCH")) {
            qWarning() << "服务器返回消息有问题";
            return false;
        }

        int index[3];
        index[0] = msg.indexOf("From: ");
        index[1] = msg.indexOf("Subject: ");
        index[2] = msg.indexOf("Date: ");
        std::sort(index, index+3);

        extractMailInfos(msg.mid(index[0], index[1]-index[0]), mail);
        extractMailInfos(msg.mid(index[1], index[2]-index[1]), mail);
        extractMailInfos(msg.mid(index[2], msg.indexOf("\r\n\r\n")-index[2]+2), mail);

        mail.to = this->id;
        mail.messageId = id;

        qDebug() << "第 " << id << " 封邮件在 " << mail.date << " 发送自 " << mail.from << " ，主题是 " << mail.subject;

        return true;
    }
    return false;
}

bool NewSocket::getMailsOfBox(QVector<EmailInfo> &mails, QString boxName)
{
    if (!(boxName.at(0).isLetter() && boxName.at(0).unicode() <= 127)) {
        boxName = Utils::matchBoxName(boxName);
    }

    if (boxName == "INBOX") {  // 收件箱
        sendCommand("2 SELECT INBOX", "imap.qq.com");
        QString msg = "";
        if (sslSocket->waitForReadyRead()) {
            msg = sslSocket->readAll();
            qDebug() << "-- 服务器：" << msg;
        } else {
            qWarning() << "2 SELECT INBOX 命令服务器返回失败";
            return false;
        }

        // 提取“总邮件数”
        int total = 0;
        QRegularExpression regex(R"(\* (\d+) EXISTS)");
        match = regex.match(msg);
        if (match.hasMatch()) {
            total = match.captured(1).toInt();  // 将提取的数字转换为整型
        }
        if (total == 0) {
            qWarning() << boxName << " 中没有邮件";
            return false;
        }

        EmailInfo mail;
        for (int i = 1; i <= total; i++) {
            // a1 FETCH x BODY[header.fields (Date From Subject)]  单条邮件
            sendCommand("a1 FETCH " + QString::number(i) + " BODY[header.fields (Date From Subject)]", "imap.qq.com");
            if (sslSocket->waitForReadyRead()) {
                msg = sslSocket->readAll();
                qDebug() << "-- 服务器：" << msg;
            } else {
                qWarning() << "a1FETCH x BODY[header.fields (Date From Subject)] 命令服务器返回失败";
                return false;
            }

            // 处理服务器返回数据
            if (!msg.contains("a1 OK FETCH")) {
                qWarning() << "服务器返回消息有问题";
                return false;
            }
            // 提取关键信息
            int index[3];
            index[0] = msg.indexOf("From: ");
            index[1] = msg.indexOf("Subject: ");
            index[2] = msg.indexOf("Date: ");
            std::sort(index, index+3);
            extractMailInfos(msg.mid(index[0], index[1]-index[0]), mail);
            extractMailInfos(msg.mid(index[1], index[2]-index[1]), mail);
            extractMailInfos(msg.mid(index[2], msg.indexOf("\r\n\r\n")-index[2]+2), mail);
            mail.to = this->id;
            mail.messageId = i;
            qDebug() << "第 " << id << " 封邮件在 " << mail.date << " 发送自 " << mail.from << " ，主题是 " << mail.subject;
            mails.append(mail);
        }
    }
    return true;
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

QString NewSocket::getMailDetail(int id)
{
    sendCommand("a6 FETCH " + QString::number(id) + " BODY[TEXT]", "imap.qq.com");
    if (sslSocket->waitForReadyRead()) {
        qDebug() << "省略这段邮件正文...";
        return sslSocket->readAll();
    }
    qWarning() << "读取邮件正文失败";
    return QString();
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

        //return Utils::extractBoxNames(message);
        QStringList boxNames = message.split("\r\n", Qt::SkipEmptyParts), res;
        QRegularExpression regex(R"(\* LIST.*\"/\" \"(.*?)\")");  // 正则表达式匹配包含文件夹名的模式
        for (const QString &boxName : boxNames) {
            match = regex.match(boxName);
            if (match.hasMatch()) {
                QString name = match.captured(1); // 提取第一个捕获组，即文件夹名
                qDebug() << "Box: " << name;

                if (!Utils::containsNonEnglish(name)) {
                    name = Utils::matchBoxName(name);
                    res.append(name);
                }
            }
        }

        return res;
    } else {
        qWarning() << "newsocket: 没有读到 imap 回复";
        return QStringList();
    }
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

bool NewSocket::disConnect(QString server)
{
    if (server == "stmp.qq.com") {
        tcpSocket->disconnect();
    } else if (server == "imap.qq.com") {
        sslSocket->disconnect();
    } else {
        qWarning() << "服务器名称无法识别";
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

//将邮件标题转换为正常字符
QString NewSocket::parseSubject(QString mimeStr,QString charset){
    static const QRegularExpression regex(R"(=\?(\S+?)\?(\S)\?(\S+?)\?=)");
    QRegularExpressionMatch match = regex.match(mimeStr);

    if (!match.hasMatch()) {
        qWarning() << "Invalid MIME encoded string";
        return QString();
    }

    charset = match.captured(1);
    QString encoding = match.captured(2).toUpper();
    QString encodedText = match.captured(3);

    QByteArray byteArray;

    if (encoding == "B") {
        byteArray = QByteArray::fromBase64(encodedText.toUtf8());
    } else if (encoding == "Q") {
        byteArray = QByteArray::fromPercentEncoding(encodedText.toUtf8());
    } else {
        qWarning() << "Unsupported encoding:" << encoding;
        return QString();
    }

    QStringDecoder decoder(charset.toStdString().c_str());
    if (!decoder.isValid()) {
        qWarning() << "Unsupported charset:" << charset;
        return QString();
    }

    return decoder.decode(byteArray);
}

//将gbk邮件转码为uff8
QString NewSocket::gbkBase64ToUtf8(QString base64,QString charset){
    QByteArray decodeBytes = QByteArray::fromBase64(base64.toUtf8());
    QStringDecoder decoder(charset.toStdString().c_str());
    QString gbkString = decoder.decode(decodeBytes);

    // 将 QString 转换为 UTF-8 编码的 QByteArray
    QByteArray utf8Data = gbkString.toUtf8();
    QString utf8String = QString::fromUtf8(utf8Data);
    return utf8String;

}

bool NewSocket::extractMailInfos(QString line, EmailInfo &info)
{
    if (line.contains("From: ")) {
        QString str = line.mid(6, line.length()-8);
        decodeMimeText(str);
        str.remove("\r\n");
        info.from = str;
    } else if (line.contains("Subject: ")) {
        QString str = line.mid(9, line.length()-11);
        decodeMimeText(str);
        str.remove("\r\n");
        info.subject = str;
    } else if (line.contains("Date: ")) {
        info.date = line.mid(6).remove("\r\n");
    } else {
        qWarning() << "line 获取错误";
        return false;
    }

    return true;
}

void NewSocket::decodeMimeText(QString &encodedText)
{
    encodedText = Utils::tryDecode(encodedText);
    // while (encodedText.contains("=?utf-8?B?")) {  //
    //     int beg = encodedText.indexOf("=?utf-8?B?") + 10;
    //     int len = encodedText.indexOf("?=") - beg;
    //     if (len < encodedText.length()-12) {
    //         QString after = Utils::decodeBase64(encodedText.mid(beg, len));
    //         QString before = encodedText.mid(beg-10, len+12);
    //         encodedText = encodedText.replace(before, after);
    //     } else {
    //         encodedText = Utils::decodeBase64(encodedText.mid(beg, len));
    //     }
    // }
}






