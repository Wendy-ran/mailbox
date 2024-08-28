#ifndef IMAPCLIENT_H
#define IMAPCLIENT_H

#include <QTcpSocket>
#include <QString>

class ImapClient {
public:
    ImapClient(const QString &server, int port);

    void connectToServer();
    void login(const QString &username, const QString &password);
    void selectInbox();
    void fetchEmailBody(int mailId);

private:
    QTcpSocket socket;
    QString server;
    int port;

    void sendCommand(const QString &cmd);
    void readResponse();
};

#endif // IMAPCLIENT_H
