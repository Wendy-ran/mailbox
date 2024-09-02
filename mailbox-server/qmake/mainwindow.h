#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include    <QTcpServer>
#include    <QLabel>
#include <QVector>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    // 颜色
    const QString hawthornRed = "rgb(49, 101, 95)";
    const QString purple = "rgb(150, 56, 161)";

    QLabel  *labListen;         //状态栏标签
    QLabel  *labSocketState;    //状态栏标签

    QTcpServer *tcpServer;      //TCP服务器
    QTcpSocket *tcpSocket=nullptr;      //TCP通讯的Socket
    QString getLocalIP();       //获取本机IP地址

    QVector<QTextEdit*> textEdits;
    QTextEdit* curWriteEdit;

    QString workDir;  // 工作目录

    int loginStep = 0;

    void connections();
    void initUI();

    void appendText(QTextEdit* textEdit, QString text, QString color = "black");
    bool createSubfolder(const QString &workDir, const QString &subDir);
    bool saveFlag = false;
    bool createTextFile(const QString &dir, const QString &file, const QString &text);
    QString readTextFile(const QString &filePath);
    QString commands;

//protected:
//    void    closeEvent(QCloseEvent *event);   //close事件处理函数
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();      //析构函数里需要做一些处理，以保证断开与客户端的连接，并停止网络监听

private slots:
    //自定义槽函数
    void    do_newConnection();         //关联QTcpServer的newConnection()信号
    void    do_socketStateChange(QAbstractSocket::SocketState socketState);
    void    do_clientConnected();       //客户端 socket 已连接
    void    do_clientDisconnected();    //客户端 socket 已断开
    void    do_socketReadyRead();       //读取socket传入的数据

    //UI生成的
    void on_actStart_triggered();

    void on_actStop_triggered();

    //void on_actClear_triggered();

    void on_btnSend_clicked();

    void on_actHostInfo_triggered();
    void on_btnClear_clicked();

    void on_btnNewP_clicked();

    void on_btnPrevP_clicked();

    void on_btnNextP_clicked();

    void on_actChoseField_triggered();

    void onStackNumChanged();
    void onCurrentChanged(int index);

    void on_btnSave_clicked();

    void on_btnCmds_clicked();

signals:
    void stackNumChanged();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
