#include "utils.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QString>

QJsonObject Utils::readJsonFile(const QString& filePath) {
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

bool Utils::writeJsonToFile(QString filePath, QJsonObject jsonObject) {
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

QString Utils::readLineFromFile(const QString& filePath, int lineNumber) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("utils: 无法打开文件");
        return QString();  // 返回一个空的 QString
    }

    QTextStream in(&file);
    QString line;
    int currentLine = 0;

    // 读取指定行号的内容
    while (!in.atEnd()) {
        line = in.readLine();
        if (++currentLine == lineNumber) {
            file.close();
            return line;  // 返回找到的行
        }
    }

    file.close();  // 如果没有找到指定的行，则关闭文件
    return QString();  // 如果行号不存在，返回一个空的 QString
}

bool Utils::containsNonEnglish(const QString &text) {
    // 创建一个正则表达式匹配非英文字母非空格字符
    QRegularExpression regex("[^a-zA-Z ]");  // 添加空格在允许的字符集中
    // 检查是否有匹配
    QRegularExpressionMatch match = regex.match(text);
    return match.hasMatch();  // 如果有匹配，返回 true
}

QString Utils::decodeBase64(const QString &base64EncodedText) {
    // 从 Base64 编码的 QString 创建 QByteArray
    QByteArray decodedByteArray = QByteArray::fromBase64(base64EncodedText.toUtf8());

    // 将 QByteArray 转换回 QString（假设原始文本是 UTF-8 编码的）
    //QString temp = QString::fromUtf8(decodedByteArray);
    return QString::fromUtf8(decodedByteArray);
}

// QStringList Utils::extractBoxNames(QString msg)
// {
//     QStringList boxNames = msg.split("\r\n", Qt::SkipEmptyParts), res;

//     //QList<QString> folderNames;
//     QRegularExpression regex(R"(\* LIST.*\"/\" \"(.*?)\")");  // 正则表达式匹配包含文件夹名的模式

//     for (const QString &boxName : boxNames) {
//         QRegularExpressionMatch match = regex.match(boxName);
//         if (match.hasMatch()) {
//             QString name = match.captured(1); // 提取第一个捕获组，即文件夹名
//             qDebug() << "Box: " << name;

//             if (!Utils::containsNonEnglish(name)) {
//                 name = matchBoxName(name);
//                 res.append(name);
//             }
//         }
//     }

//     return res;
// }

QString Utils::matchBoxName(QString origin)
{
    if (origin.isEmpty()) {
        qWarning() << "提供的 origin 为空";
        return "";
    }

    QString ret = "";

    if (origin.at(0).isLetter() && origin.at(0).unicode() <= 127) {
        if (origin == "INBOX") {
            ret = "收件箱";
        } else if (origin == "Sent Messages") {
            ret = "已发送";
        } else if (origin == "Drafts") {
            ret = "草稿箱";
        } else if (origin == "Deleted Messages") {
            ret = "垃圾箱";
        } else if (origin == "Junk") {
            ret = "垃圾邮件";
        } else {
            qWarning() << "newsocket: 不匹配任何邮箱文件名";
        }
    } else {
        if (origin == "收件箱") {
            ret = "INBOX";
        } else if (origin == "已发送") {
            ret = "Sent Messages";
        } else if (origin == "草稿箱") {
            ret = "Drafts";
        } else if (origin == "垃圾箱") {
            ret = "Deleted Messages";
        } else if (origin == "垃圾邮件") {
            ret = "Junk";
        } else {
            qWarning() << "newsocket: 不匹配任何邮箱文件名";
        }
    }

    return ret;
}

QString Utils::tryDecode(const QString &text) {
    QRegularExpression regex(R"(=\?(.+?)\?(B|Q)\?(.+?)\?=)",
                             QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = regex.match(text);
    if (match.hasMatch()) {
        QString charset = match.captured(1);   // 字符集，例如 "UTF-8"
        QString encoding = match.captured(2);  // 编码方式，"B" 表示 Base64
        QString encodedText = match.captured(3); // 编码后的文本

        QByteArray decodedBytes;
        if (encoding == "B") {
            decodedBytes = QByteArray::fromBase64(encodedText.toLatin1());
        } else if (encoding == "Q") {
            // 如果需要处理 Quoted-Printable 编码，请在这里实现
        }

        // 使用QString构造函数直接从UTF-8字节数据创建字符串
        if (charset.toLower() == "utf-8") {
            return QString::fromUtf8(decodedBytes);
        } else {
            // 如果字符集不是UTF-8，这里可以根据需要添加其他字符集的支持
            return QString::fromLatin1(decodedBytes); // 假设是Latin1，这取决于实际情况
        }
    }
    return text;  // 如果没有匹配或解码失败，返回原始文本
}



