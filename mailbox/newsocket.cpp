#include "newsocket.h"
#include <QThread>
#include <QSslError>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
//#include "mainwindow.h"
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

bool NewSocket::getMailofBox(EmailInfo &mail, QString boxName, int id)
{
    if (!(boxName.at(0).isLetter() && boxName.at(0).unicode() <= 127)) {
        boxName = Utils::matchBoxName(boxName);
    }

    if (boxName == "INBOX") {
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

        // 100 FETCH 1:16 BODY[header.fields (Date From Subject)]
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
        // 这个变量和迭代器的赋值规范一样
        // int beg = msg.indexOf("\r\n", 0)+2, end = msg.indexOf("\r\n\r\n", beg);
        // //msg = msg.mid(beg, end-beg);
        // for (int i = beg; i < end; i++) {
        //     if (msg.at(i) == 'F') {

        //     }
        // }

        int index[3];
        index[0] = msg.indexOf("From: ");
        index[1] = msg.indexOf("Subject: ");
        index[2] = msg.indexOf("Date: ");
        std::sort(index, index+3);

        extractMailInfos(msg.mid(index[0], index[1]-index[0]), mail);
        extractMailInfos(msg.mid(index[1], index[2]-index[1]), mail);
        extractMailInfos(msg.mid(index[2], msg.indexOf("\r\n\r\n")-index[2]+2), mail);

        // QStringList lines = msg.split("\r\n\r\n", Qt::SkipEmptyParts);
        // for (QString &line : lines) {
        //     if (line.contains("a0 OK FETCH Completed")) {
        //         continue;
        //     }
            // 示例邮件头数据
            // QString data = "Date: Thu, 25 Jul 2024 02:09:05 +0000 (GMT)\n"
            //                "From: iCloud <noreply@icloud.com.cn>\n"
            //                "Subject: =?UTF-8?B?5L2g55qEIGlDbG91ZCDlgqjlrZjnqbrpl7Tlt7Lmu6E=?=";

            // 使用 QRegularExpression 提取日期、发件人和主题
            // QRegularExpression dateRegex("Date: (.+)");
            // QRegularExpression fromRegex("From: (.+)");
            // QRegularExpression subjectRegex("Subject: (.+)");

            // QRegularExpressionMatch dateMatch = dateRegex.match(line);
            // QRegularExpressionMatch fromMatch = fromRegex.match(line);
            // QRegularExpressionMatch subjectMatch = subjectRegex.match(line);

            // QString date = dateMatch.hasMatch() ? dateMatch.captured(1) : "";
            // QString from = fromMatch.hasMatch() ? fromMatch.captured(1) : "";
            // QString subject = subjectMatch.hasMatch() ? subjectMatch.captured(1) : "";


            /* 2 FETCH (BODY[HEADER.FIELDS (DATE FROM SUBJECT)] {212}\r\n

            * From: \"=?utf-8?B?UVHpgq7nrrHlm6LpmJ8=?=\" <10000@qq.com>\r\n

            * Subject: =?utf-8?B?IkZveG1haWwi5bey6I635b6X5LqG5L2g55qEUVE=?=\r\n =?utf-8?B?6YKu566x6LSm5Y+355qE6K6/6Zeu5p2D6ZmQ?=\r\n

            * Date: Sat, 27 Jul 2024 10:20:18 +0800\r\n\r\n)\r\na0 OK FETCH Completed\r\n
            */
            // From Subject Date 出现的顺序不确定


            // mail.date = date;
            // mail.from = from;
            // mail.subject = subject;
        mail.to = this->id;
        mail.messageId = id;

        qDebug() << "第 " << id << " 封邮件在 " << mail.date << " 发送自 " << mail.from << " ，主题是 " << mail.subject;

            // 解码主题
            // QString subject;
            // if (!encodedSubject.isEmpty()) {
            //     QByteArray encodedBytes = encodedSubject.toUtf8();
            //     QByteArray decodedBytes = QByteArray::fromBase64(encodedBytes.mid(10, encodedBytes.size() - 12)); // 从 MIME 编码提取 Base64 部分
            //     subject = QTextCodec::codecForName("UTF-8")->toUnicode(decodedBytes);
            // }

            // qDebug() << "Date:" << date;
            // qDebug() << "From:" << from;
            // qDebug() << "Subject:" << subject;
        //}
        return true;
    }
    return false;

}

// QString NewSocket::parseAddress(const QString &address) {
//     // 正确处理转义和字符串格式
//     QRegularExpression addressRegex(R"(\"([^"]*)\" NIL \"([^"]*)\" \"([^"]*)\")");
//     QRegularExpressionMatch match = addressRegex.match(address);
//     if (match.hasMatch()) {
//         return QString("%1@%2").arg(match.captured(2), match.captured(3));
//     }
//     return QString();
// }

// bool NewSocket::reciviveByEmailId(int num, Mail& mail, QString boxName)
// {
//     if (!boxName.at(0).isLetter()) {
//         boxName = Utils::matchBoxName(boxName);
//     }

//     QVector<NewSocket::EmailInfo> emails;
//     // INBOX
//     sendCommand("2 SELECT INBOX", "imap.qq.com");
//     QString msg = "";
//     if (sslSocket->waitForReadyRead()) {
//         msg = sslSocket->readAll();
//         qDebug() << "-- 服务器：" << msg;
//     } else {
//         qWarning() << "2 SELECT INBOX 命令服务器返回失败";
//         return false;
//         //return emails;
//     }

//     // 提取“总邮件数”
//     int total = 0;
//     QRegularExpression regex(R"(\* (\d+) EXISTS)");
//     match = regex.match(msg);
//     if (match.hasMatch()) {
//         total = match.captured(1).toInt();  // 将提取的数字转换为整型
//     }
//     if (total == 0) {
//         qWarning() << boxName << " 中没有邮件";
//         return false;
//         //return emails;
//     }

//     //sendCommand("3 FETCH 1:" + QString::number(total) + " (ENVELOPE)", "imap.qq.com");
//     sendCommand("A0a FETCH " + QString::number(num) + " RFC822", "imap.qq.com");
//     //imap.send(QString::fromUtf8("A0a FETCH ").append( QString::number(num) ).append(" RFC822"));
//     if (sslSocket->waitForReadyRead()) {
//         msg = sslSocket->readAll();
//         qDebug() << msg;
//     } else {
//         qWarning() << "3 FETCH 1:x 命令服务器返回失败";
//         return false;
//         //return emails;
//     }
//     // parseEmailInfos
//     QStringList list = msg.split("\r\n", Qt::SkipEmptyParts);

//     // QString str = Utils::readLineFromFile(":/config/something.txt", 2);
//     // str = R"raw(\* \d+ FETCH \(ENVELOPE \("([^"]+)" "([^"]+)" \(([^)]+?\)*)\) \(([^)]+?\)*)\) \(([^)]+?\)*)\) \(([^)]+?\)*)\) NIL NIL NIL ""\)\))raw";
//     // const QString complexRegexStr = R"raw(\* \d+ FETCH \(ENVELOPE \("([^"]+)" "([^"]+)" )raw"
//     //                                 R"raw(\((?:[^()]+|\([^()]*\))*\) )raw"  // 处理可能的嵌套括号
//     //                                 R"raw(\((?:[^()]+|\([^()]*\))*\) )raw"
//     //                                 R"raw(\((?:[^()]+|\([^()]*\))*\) )raw"
//     //                                 R"raw(\((?:[^()]+|\([^()]*\))*\) )raw"
//     //                                 R"raw(NIL NIL NIL ""\)\))raw";
//     // QRegularExpression envelopeRegex(complexRegexStr);

//     // int cnt = 0;
//     // for (const QString &line : lines) {
//     //     qDebug() << "-- 服务器：" << line;

//     //     match = envelopeRegex.match(line);
//     //     if (match.hasMatch()) {
//     //         EmailInfo info;
//     //         info.date = match.captured(1);
//     //         info.subject = Utils::decodeBase64(match.captured(2));
//     //         info.from = parseAddress(match.captured(3));
//     //         info.to = parseAddress(match.captured(4));
//     //         // Assuming CC and BCC are not provided in this data
//     //         info.messageId = match.captured(10);

//     //         emails.append(info);

//     //         cnt++;
//     //         qDebug() << "第 " << cnt << " 封邮件在 " << info.date << " 发送自 " << info.from << " ，主题是 " << info.subject;
//     //     } else {
//     //         qWarning() << "No match found";
//     //     }
//     // }
//     // return emails;

//     //QStringList list = imap.data.split("\r\n");  lines
//     //Mail mail;
//     QString charset;
//     for (QStringList::Iterator iter = list.begin(); iter!=list.end();++iter){
//         if((*iter).contains("charset=\"")){
//             charset = *iter;
//             QStringList list = charset.split("\"");
//             charset = list[1];
//         }else if((*iter).contains("charset=")){
//             charset = *iter;
//             QStringList list = charset.split("=");
//             charset = list[1];
//         }

//         if((*iter).contains("Subject")){
//             if((*iter).contains("?B?")){
//                 QString text = (*iter);
//                 text = parseSubject(text,charset);
//                 mail.subject = text;
//             }else{
//                 QString subject = *iter;
//                 subject = subject.split(":")[1];
//                 subject = subject.trimmed();

//                 mail.subject = subject;
//             }
//         }

//         if((*iter).contains("From:")){
//             QString text =*iter;

//             if(text.contains("?B?")){
//                 QRegularExpression regex(R"(<(\S+?)>)");
//                 QString prev = regex.match(text).captured(1);
//                 text = parseSubject(*iter,charset);
//                 text = text + ": " + prev;
//             }
//             mail.from_email = text;
//         }

//         if((*iter).contains("To:")){
//             QString text =*iter;
//             if(text.contains("?B?")){
//                 QRegularExpression regex(R"(<(\S+?)>)");
//                 QString prev = regex.match(text).captured(1);
//                 text = parseSubject(*iter,charset);
//                 text = text + ": " + prev;
//             }
//             mail.to_email = text;
//         }
//     }

//     if(msg.contains("Mails not exist!")){
//         qWarning() << "请求的邮件不存在";
//         return false;
//     }

//     QString nohtml;
//     QString html;
//     static const QRegularExpression re0(R"(Content-Transfer-Encoding: base64\s+(\s\S+?)\s+?------=_Part_)");
//     QRegularExpressionMatch ma0 = re0.match(msg);
//     if (ma0.hasMatch()) {
//         nohtml = ma0.captured(1);
//     }

//     static const QRegularExpression re1(R"(Content-Type: text/html; \S+?\s+?Content-Transfer-Encoding: base64\s+(\S+\s*?\S+?)\s+?------=_Part_)");
//     QRegularExpressionMatch ma1 = re1.match(msg);
//     if (ma1.hasMatch()) {
//         html = ma1.captured(1);
//     }

//     nohtml = gbkBase64ToUtf8(nohtml,charset);
//     nohtml = nohtml.trimmed();
//     mail.noHtmlText = nohtml;
//     html = gbkBase64ToUtf8(html,charset);
//     html = html.trimmed();
//     mail.text = html;

//     // 打印检查
//     qDebug() << "第 " << num << " 封邮件发送自 " << mail.from_email << " ，主题是 " << mail.subject << " 正文是：" << mail.text;
//     qDebug() << "nohtml: " << mail.noHtmlText;

//     return true;
// }

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






