#ifndef NEWSOCKET_H
#define NEWSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QSslSocket>
#include <QDebug>
#include <QString>
#include <QRegularExpressionMatch>
#include <QFile>

class NewSocket : public QObject
{
    Q_OBJECT

public:
    struct EmailInfo {
        QString date = "";
        QString subject = "";
        QString from = "";
        QString to = "";
        QString cc = "";
        QString bcc = "";
        int messageId = -1;
        QString uid = "";
    };

private:
    QTcpSocket *tcpSocket;
    QSslSocket *sslSocket;

    // 标志
    bool flag = true;
    //bool authFlag = false;
    //bool loginFlag = false;
    //bool connectFlag = false;

    QString id = "";
    QString code = "";

    //QString strBoxName;
    QRegularExpressionMatch match;
    //QString parseAddress(const QString &address);
    QString parseSubject(QString mimeStr,QString charset);
    QString gbkBase64ToUtf8(QString base64,QString charset);
    bool extractMailInfos(QString line, EmailInfo& info);
    void decodeMimeText(QString &encodedText);

public:

    // struct Mail{
    //     QList<QFile*> fileList;
    //     QString from_email;
    //     QString to_email;
    //     QString subject;
    //     QString text;
    //     QString noHtmlText;
    //     QString toString();
    // };

    // QDebug operator<<(QDebug &debug, const NewSocket::EmailInfo &info) {
    //     // 实现详情
    //     return debug;
    // }

    //NewSocket();
    NewSocket(QObject *parent = nullptr);
    ~NewSocket();

    bool tryConnection(QString server, int num = 5);
    void sendCommand(QString command, QString serverName);
    bool login(QString id, QString code);
    //bool readResponse();
    bool sendEmail(const QString& from, const QString& to,
                   const QString& subject, const QString& body);
    bool writeLetter(const QString &from, const QString &to, const QString &subject, const QString &body);

    QStringList getBoxNames();

    // 收取历史邮件  多个重载函数
    //bool getHisLetters();

    QString getId();
    QString getCode();
    QTcpSocket* getTcpSocket();
    QSslSocket* getSslSocket();
    bool socketState(QString socketName);
    //void printVar();
    void connections();
    //QString matchBoxName(QString origin);
    //QVector<EmailInfo> getMails(QString box);
    //bool reciviveByEmailId(int num, Mail& mail, QString boxName);
    bool getMailofBox(EmailInfo &mail, QString boxName, int id = -1);

private slots:
    void connectToServer();
    void disconnectFromServer();
    void getMessage();
    void onErrorConnection();
    //void onConnected();
    //void onAuth();
    void onEncrypted();
    void onSslErrors(const QList<QSslError>& msgs);
    //void onBoxNames(QString names);

signals:
    //void authSucc();
    //void gotBoxNames(QString names);
    //void transferBoxNames(QStringList names);
};

#endif // NEWSOCKET_H
