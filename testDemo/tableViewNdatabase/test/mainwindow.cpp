#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSslSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connection();
    QString msg = "";
    if (socket->waitForReadyRead()) {
        msg = socket->readAll();
        qDebug() << "===========================================";
        //qDebug() << msg;
    }

    const QString complexRegexStr = R"raw(\* \d+ FETCH \(ENVELOPE \("([^"]+)" "([^"]+)" )raw"
                                    R"raw(\((?:[^()]+|\([^()]*\))*\) )raw"  // 处理可能的嵌套括号
                                    R"raw(\((?:[^()]+|\([^()]*\))*\) )raw"
                                    R"raw(\((?:[^()]+|\([^()]*\))*\) )raw"
                                    R"raw(\((?:[^()]+|\([^()]*\))*\) )raw"
                                    R"raw(NIL NIL NIL ""\)\))raw";
    QRegularExpression complexRegex(complexRegexStr);



    QStringList lines = msg.split("\r\n", Qt::SkipEmptyParts);

    int cnt = 0;
    for (const QString &line : lines) {
        qDebug() << "-- 服务器：" << line;

        QRegularExpressionMatch match = complexRegex.match(line);
        if (match.hasMatch()) {
            EmailInfo info;
            info.date = match.captured(1);
            info.subject = decodeBase64(match.captured(2));
            info.from = parseAddress(match.captured(3));
            info.to = parseAddress(match.captured(4));
            // Assuming CC and BCC are not provided in this data
            info.messageId = match.captured(10);

            //emails.append(info);

            cnt++;
            qDebug() << "第 " << cnt << " 封邮件在 " << info.date << " 发送自 " << info.from << " ，主题是 " << info.subject;
        } else {
            qWarning() << "No match found";
        }
    }

}

QString MainWindow::decodeBase64(const QString &base64EncodedText) {
    // 从 Base64 编码的 QString 创建 QByteArray
    QByteArray decodedByteArray = QByteArray::fromBase64(base64EncodedText.toUtf8());

    // 将 QByteArray 转换回 QString（假设原始文本是 UTF-8 编码的）
    return QString::fromUtf8(decodedByteArray);
}

QString MainWindow::parseAddress(const QString &address) {
    // 正确处理转义和字符串格式
    QRegularExpression addressRegex(R"(\"([^"]*)\" NIL \"([^"]*)\" \"([^"]*)\")");
    QRegularExpressionMatch match = addressRegex.match(address);
    if (match.hasMatch()) {
        return QString("%1@%2").arg(match.captured(2), match.captured(3));
    }
    return QString();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connection()
{
    socket = new QSslSocket;
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

    command = "2 SELECT INBOX\r\n";
    socket->write(command.toUtf8());
    qDebug() << command;
    if (socket->waitForReadyRead()) {
        qDebug() << socket->readAll();
    }

    command = "3 FETCH 1:16 (ENVELOPE)\r\n";
    socket->write(command.toUtf8());
    qDebug() << command;
}
