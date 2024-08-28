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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void appendMessageWithColor(const QString &message, const QString &color);

private slots:
    void on_btn_send_clicked();

    void on_btn_connect_clicked();

private slots:
    void connectToServer();
    void disconnectFromServer();
    void getMessage();
    void onErrorConnection();

private:
    Ui::MainWindow *ui;

    QSslSocket* socket;
};
#endif // MAINWINDOW_H
