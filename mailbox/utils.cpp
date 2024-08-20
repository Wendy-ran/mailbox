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
    QString decodedText = QString::fromUtf8(decodedByteArray);

    return decodedText;
}

