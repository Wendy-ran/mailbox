#ifndef TFORMDOC_H
#define TFORMDOC_H

#include <QWidget>
#include "newsocket.h"

QT_BEGIN_NAMESPACE
namespace Ui {class TFormDoc;}
QT_END_NAMESPACE


class TFormDoc : public QWidget
{
    Q_OBJECT

private:
    QString m_filename;             //当前文件
    bool    m_fileOpened=false;     //文件已打开
    NewSocket* curNewSocket;

public:
    TFormDoc(NewSocket* newSocket, QWidget *parent = nullptr);
    ~TFormDoc();

    void    loadFromFile(QString& aFileName);   //打开文件
    QString currentFileName();  //返回当前文件名
    bool    isFileOpened();     //文件已经打开
    void    saveToFile();     //保存文件

    void    setEditFont();      //设置字体
    void    textCut();          //cut
    void    textCopy();         //copy
    void    textPaste();        //paste
    void createDirectoryAndFile(const QString &path, const QString &dirName, const QString &fileName);
    QString generateRandomNumberString(int length = 64);
    QString getCurrentFormattedTime(const QString& format = "yyyy-MM-dd-HHmmss");

    void setText(QString text);

private slots:
    void on_btnSend_clicked();

    void on_btnSave_clicked();

private:
    Ui::TFormDoc *ui;

};

#endif // QFORMDOC_H
