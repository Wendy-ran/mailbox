#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSslSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    struct EmailInfo {
        QString date;
        QString subject;
        QString from;
        QString to;
        QString cc;
        QString bcc;
        QString messageId;
    };

    QString parseAddress(const QString &address);
    QString decodeBase64(const QString &base64EncodedText);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void connection();
    QSslSocket *socket;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
