#include "tformdoc.h"
#include "ui_tformdoc.h"

#include    <QFile>
#include    <QTextStream>
#include    <QFontDialog>
#include    <QFileInfo>
#include    <QMessageBox>
#include <QDir>
#include <QString>
#include <QRandomGenerator>
#include <QDateTime>

QString TFormDoc::getCurrentFormattedTime(const QString& format) {
    QDateTime currentTime = QDateTime::currentDateTime();  // 获取当前日期和时间
    return currentTime.toString(format);  // 格式化并返回日期和时间
}

QString TFormDoc::generateRandomNumberString(int length) {
    QString result;
    result.reserve(length);  // 预留足够的空间

    // 使用 QRandomGenerator 生成随机数字
    for (int i = 0; i < length; ++i) {
        int number = QRandomGenerator::global()->bounded(10);  // 生成 0 到 9 的数字
        result.append(QString::number(number));
    }

    return result;
}

TFormDoc::TFormDoc(NewSocket* newSocket, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TFormDoc),
    curNewSocket(newSocket)
{
    ui->setupUi(this);

    this->setWindowTitle("New Doc[*]");    //窗口标题
    this->setAttribute(Qt::WA_DeleteOnClose);   //关闭时自动删除
    connect(ui->plainTextEdit, &QPlainTextEdit::modificationChanged,
            this, &QWidget::setWindowModified);

    ui->comboSender->addItem(curNewSocket->getId());
}

TFormDoc::~TFormDoc()
{
//    QMessageBox::information(this,"信息","文档窗口被释放");
    delete ui;
}

void TFormDoc::loadFromFile(QString &aFileName)
{//打开文件
    QFile aFile(aFileName);     //以文件方式读出
    if (aFile.open(QIODevice::ReadOnly | QIODevice::Text)) //以只读文本方式打开文件
    {
        QTextStream aStream(&aFile);    //用文本流读取文件
        ui->plainTextEdit->clear();
        ui->plainTextEdit->setPlainText(aStream.readAll()); //读取文本文件
        aFile.close();

        m_filename=aFileName;             //保存当前文件名
        QFileInfo   fileInfo(aFileName);    //文件信息
        QString str=fileInfo.fileName();    //去除路径后的文件名
        this->setWindowTitle(str+"[*]");
        m_fileOpened=true;
    }
}

QString TFormDoc::currentFileName()
{
    return  m_filename;
}

bool TFormDoc::isFileOpened()
{ //文件是否已打开
    return m_fileOpened;
}

void TFormDoc::saveToFile()
{
//没有执行具体的保存操作
    this->setWindowModified(false);
}

void TFormDoc::setEditFont()
{
    QFont   font;
    font=ui->plainTextEdit->font();

    bool    ok;
    font=QFontDialog::getFont(&ok,font);
    ui->plainTextEdit->setFont(font);
}

void TFormDoc::textCut()
{
    ui->plainTextEdit->cut();
}

void TFormDoc::textCopy()
{
    ui->plainTextEdit->copy();
}

void TFormDoc::textPaste()
{
    ui->plainTextEdit->paste();
}

void TFormDoc::on_btnSend_clicked()
{
    //TODO 检查表单的数据是否合法
    bool isOk = curNewSocket->sendEmail(curNewSocket->getId(), ui->lineEditReceiver->text(),
                            ui->lineEditTheme->text(), ui->plainTextEdit->toPlainText());
    if (isOk) {
        qDebug() << "邮件成功发送";
    } else {
        qDebug() << "邮件发送失败";
        qDebug() << "curNewSocket->getId() - " << curNewSocket->getId();
        qDebug() << "ui->lineEditReceiver->text() - " << ui->lineEditReceiver->text();
    }
}



void TFormDoc::createDirectoryAndFile(const QString &path, const QString &dirName, const QString &fileName) {
    QDir dir(path);  // 创建一个QDir实例指向需要的路径

    // 检查目标目录是否存在，如果不存在则创建
    if (!dir.exists(dirName)) {
        if (dir.mkdir(dirName)) {
            qDebug() << "Directory created successfully.";
        } else {
            qDebug() << "Failed to create directory.";
            return;  // 如果创建目录失败，直接返回
        }
    } else {
        qDebug() << "Directory already exists.";
    }

    // 设置文件的完整路径
    QString filePath = QDir::cleanPath(dir.absoluteFilePath(dirName + "/" + fileName));

    // 创建并打开文件
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        //out << "Hello, world!";  // 写入一些文本
        out << this->ui->plainTextEdit->toPlainText();
        qDebug() << this->ui->plainTextEdit->toPlainText();
        qDebug() << "File written successfully.";
        file.close();
    } else {
        qDebug() << "Failed to open file for writing.";
    }
}

void TFormDoc::on_btnSave_clicked()
{
    QString baseDir = "D://OneDrive - AuroraNotebook//codeField//projects//mail//mine//files//344500";
    QString dirName = getCurrentFormattedTime();
    QString fileName = generateRandomNumberString() + ".txt";
    createDirectoryAndFile(baseDir, dirName, fileName);
}

