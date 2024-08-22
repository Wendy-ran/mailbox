#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDate>

class Utils {
public:    
    static QJsonObject readJsonFile(const QString& filePath);
    static bool writeJsonToFile(QString filePath, QJsonObject jsonObject);
    static QString readLineFromFile(const QString& filePath, int lineNumber);
    static bool containsNonEnglish(const QString &text);
    static QString decodeBase64(const QString &base64EncodedText);
    static QString matchBoxName(QString origin);
    static QString tryDecode(const QString &text);

    // 分析 QQ 服务器返回的消息
    //static QStringList extractBoxNames(QString msg);
    //static QVector<EmailInfo> parseEmailInfos(const QStringList &responseList);
};

#endif // UTILS_H

