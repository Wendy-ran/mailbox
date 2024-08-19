#ifndef NEWSOCKET_H
#define NEWSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QSslSocket>

class NewSocket : public QObject
{
    Q_OBJECT

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

public:
    //NewSocket();
    NewSocket(QObject *parent = nullptr);
    ~NewSocket();

    bool tryConnection(QString server, int num = 5);
    void sendCommand(QString command, QString serverName);
    bool login(QString id, QString code);
    bool readResponse();
    bool sendEmail(const QString& from, const QString& to,
                   const QString& subject, const QString& body);
    bool writeLetter(const QString &from, const QString &to, const QString &subject, const QString &body);

    bool getBoxNames();
    // 收取历史邮件  多个重载函数
    bool getHisLetters();

    QString getId();
    QString getCode();
    QTcpSocket* getTcpSocket();
    QSslSocket* getSslSocket();
    bool socketState(QString socketName);
    //void printVar();
    void connections();
    QString matchBoxName(QString origin);

private slots:
    void connectToServer();
    void disconnectFromServer();
    void getMessage();
    void onErrorConnection();
    //void onConnected();
    //void onAuth();
    void onEncrypted();
    void onSslErrors(const QList<QSslError>& msgs);
    void onBoxNames(QString names);

signals:
    //void authSucc();
    void gotBoxNames(QString names);
    void transferBoxNames(QStringList names);
};

#endif // NEWSOCKET_H
