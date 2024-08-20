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
};

#endif // UTILS_H

