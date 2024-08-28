#include "ImapClient.h"
#include <QTextStream>
#include <QDebug>

ImapClient::ImapClient(const QString &server, int port)
    : server(server), port(port) {}

void ImapClient::connectToServer() {
    socket.connectToHost(server, port);
    if (!socket.waitForConnected(5000)) {
        qDebug() << "Connection failed:" << socket.errorString();
        return;
    }
    readResponse();
}

void ImapClient::login(const QString &username, const QString &password) {
    sendCommand("LOGIN " + username + " " + password);
    readResponse();
}

void ImapClient::selectInbox() {
    sendCommand("SELECT INBOX");
    readResponse();
}

void ImapClient::fetchEmailBody(int mailId) {
    sendCommand(QString("FETCH %1 BODY[TEXT]").arg(mailId));
    readResponse();
}

void ImapClient::sendCommand(const QString &cmd) {
    if (socket.isOpen()) {
        QTextStream stream(&socket);
        stream << cmd << "\r\n";
        stream.flush();
        qDebug() << "Command sent:" << cmd;
    }
}

void ImapClient::readResponse() {
    while (socket.waitForReadyRead(5000)) {
        qDebug() << "Response:" << socket.readAll();
    }
}
