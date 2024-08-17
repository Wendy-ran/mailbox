#ifndef NEWSOCKET_H
#define NEWSOCKET_H

#include <QObject>
#include <QTcpSocket>

class NewSocket : public QObject
{
    Q_OBJECT

private:
    QTcpSocket *socket;

    // 标志
    bool flag = true;
    //bool authFlag = false;
    //bool loginFlag = false;
    //bool connectFlag = false;

    QString id = "";
    QString code = "";

public:
    //NewSocket();
    NewSocket(QObject *parent = nullptr);
    ~NewSocket();

    bool tryConnection(int num);    
    void sendCommand(QString command);
    bool login(QString id, QString code);
    bool readResponse();
    bool sendEmail(const QString& from, const QString& to,
                   const QString& subject, const QString& body);
    bool writeLetter(const QString &from, const QString &to, const QString &subject, const QString &body);

    QString getId();
    QString getCode();
    QTcpSocket* getSocket();
    bool socketState();
    //void printVar();
    void connections();

private slots:
    void connectToServer();
    void disconnectFromServer();
    void getMessage();
    void onErrorConnection();
    //void onConnected();
    //void onAuth();

signals:
    //void authSucc();
};

#endif // NEWSOCKET_H
