#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDate>

class Utils {
public:
    static QJsonObject readJsonFile(const QString& filePath);
    static bool writeJsonToFile(QString filePath, QJsonObject jsonObject);

};

#endif // UTILS_H

