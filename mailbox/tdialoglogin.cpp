#include "tdialoglogin.h"
#include "ui_tdialoglogin.h"

#include    <QMouseEvent>
#include    <QSettings>
#include    <QMessageBox>
#include    <QByteArray>
#include    <QCryptographicHash>
#include <QDebug>
#include <stdexcept>
#include <QThread>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QIODevice>

QJsonObject TDialogLogin::readJsonFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Cannot open file for reading");
        return {};
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) {
        qWarning("JSON document is not an object");
        return {};
    }

    return doc.object();
}

bool TDialogLogin::writeJsonToFile(QString filePath, QJsonObject jsonObject) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }

    QJsonDocument document(jsonObject);
    file.write(document.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

void TDialogLogin::addId(QString str)
{
    qDebug() << "wanted to add " << str;
    QJsonObject jsonObject = readJsonFile(":/config/conf.json");
    if (jsonObject.contains("userIds") && jsonObject["userIds"].isArray()) {
        QJsonArray userIds = jsonObject["userIds"].toArray();
        for (const QJsonValue& value : userIds) {
            if (value.toString() == str) {
                return;
            }
        }
        userIds.append(str);
        jsonObject["userIds"] = userIds;
    } else {
        QJsonArray newArray;
        newArray.append(str);
        jsonObject["userIds"] = newArray;
    }

    // TODO: 动态地址
    if (!writeJsonToFile("D://OneDrive - AuroraNotebook//codeField//"
                         "projects//mail//mine//3.1//mailbox//conf.json", jsonObject)) {
        qDebug() << "Failed to write JSON to file";
    } else {
        qDebug() << "ID added successfully";
    }
}

void TDialogLogin::loadHisIds()
{
    // 读取 JSON 文件
    QJsonObject jsonObject = readJsonFile(":/config/conf.json");
    if (jsonObject.isEmpty()) {
        qWarning("JSON object is empty or file could not be read.");
        return; // 或者处理错误情况
    }

    if (jsonObject.contains("userIds") && jsonObject["userIds"].isArray()) {
        ui->editUser->clear();
        QJsonArray userIdsArray = jsonObject["userIds"].toArray();
        for (const QJsonValue &value : userIdsArray) {
            qDebug() << "User ID:" << value.toString();
            ui->editUser->addItem(value.toString());
        }
    }
}

//鼠标按键被按下
void TDialogLogin::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_moving = true;
        //m_lastPos = event->globalPosition().toPoint() - this->pos();
        //m_lastPos = event->globalPos() - this->pos();
        #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            m_lastPos = event->globalPosition().toPoint() - this->pos();  // Qt 6
        #else
            m_lastPos = event->globalPos() - this->pos();  // Qt 5
        #endif
    }
    return QDialog::mousePressEvent(event);
}

//鼠标按下左键移动
void TDialogLogin::mouseMoveEvent(QMouseEvent *event)
{
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QPoint eventPos=event->globalPosition().toPoint();
    #else
        QPoint eventPos = event->globalPos();  // Qt 5
    #endif

    if (m_moving && (event->buttons() & Qt::LeftButton)
        && (eventPos-m_lastPos).manhattanLength() > QApplication::startDragDistance())
    {
        move(eventPos-m_lastPos);
        m_lastPos = eventPos - this->pos();
    }
    return QDialog::mouseMoveEvent(event);
}

//鼠标按键被释放
void TDialogLogin::mouseReleaseEvent(QMouseEvent *event)
{
    m_moving=false;     //停止移动
    event->accept();
}

// TDialogLogin::TDialogLogin(QWidget *parent) :
//     QDialog(parent),
//     ui(new Ui::TDialogLogin)
// 构造函数
TDialogLogin::TDialogLogin(NewSocket *newSocket, QWidget *parent)
    : QDialog(parent),
    m_newSocket(newSocket),
    ui(new Ui::TDialogLogin)
{
    ui->setupUi(this);

    if (!newSocket) {
        throw std::invalid_argument("newSocket parameter cannot be nullptr");
    }

    ui->editPSWD->setEchoMode(QLineEdit::Password);     //设置为密码输入模式
    this->setAttribute(Qt::WA_DeleteOnClose);           //对话框关闭时自动删除
    this->setWindowFlags(Qt::SplashScreen);             //设置为SplashScreen, 窗口无边框,不在任务栏显示
    // this->setWindowFlags(Qt::FramelessWindowHint);    //无边框，但是在任务显示对话框标题

    // 初始化注册表
    // HKEY_CURRENT_USER\Software\<OrganizationName>\<ApplicationName>
    // 在 Windows 系统中，“组织名称”通常用作注册表中的一个顶级键
    QApplication::setOrganizationName("Qt-prjs");
    // 这个名称代表应用名称，通常用作组织键下的子键，用于存储该应用程序的特定设置
    QApplication::setApplicationName("mailbox");

    // 加载配置文件中的历史账号
    loadHisIds();
}

TDialogLogin::~TDialogLogin()
{
    delete ui;
}

QString TDialogLogin::getId()
{
    return user;
}

QString TDialogLogin::getCode()
{
    return pswd;
}

//"确定"按钮响应
void TDialogLogin::on_btnOK_clicked()
{
    user = ui->editUser->currentText().trimmed() + "@qq.com";    //输入的用户名
    pswd = ui->editPSWD->text().trimmed();    //输入的密码
    //QString encrptPSWD=encrypt(pswd);   //对输入的密码进行加密
    if (m_newSocket->login(user, pswd)) {
        this->accept();
    } else {
        this->reject();
    }

    if (ui->chkBoxSave->isChecked()) {
        // 更新账号列表
        addId(ui->editUser->currentText());
    }
}
