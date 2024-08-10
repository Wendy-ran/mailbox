#ifndef TDIALOGLOGIN_H
#define TDIALOGLOGIN_H

#include <QDialog>
//#include <QTcpSocket>
#include "newsocket.h"

QT_BEGIN_NAMESPACE
namespace Ui {class TDialogLogin;}
QT_END_NAMESPACE


class TDialogLogin : public QDialog
{
    Q_OBJECT

private:
    bool    m_moving=false;     //表示窗口是否在鼠标操作下移动
    QPoint  m_lastPos;          //上一次的鼠标位置

    QString m_user="";      //初始化用户名  unused
    QString m_pswd="";     //初始化密码，未加密的  unused

    int m_tryCount=0; //试错次数

    void loadHisIds();
    QJsonObject readJsonFile(const QString& filePath);
    void addId(QString str);
    bool writeJsonToFile(QString filePath, QJsonObject jsonObject);

    //QTcpSocket *m_tcpSocket;
    NewSocket *m_newSocket;
    QString user;
    QString pswd;

protected:
//鼠标事件处理函数，用于拖动窗口
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public:
    //TDialogLogin(QWidget *parent = nullptr);
    //explicit TDialogLogin(QTcpSocket *tcpSocket, QWidget *parent = nullptr); // 修改构造函数
    explicit TDialogLogin(NewSocket *newSocket, QWidget *parent = nullptr);
    ~TDialogLogin();

    QString getId();
    QString getCode();

private slots:
    void on_btnOK_clicked();    //"确定"按钮

signals:
    void connected();

private:
    Ui::TDialogLogin *ui;
};

#endif // TDIALOGLOGIN_H
