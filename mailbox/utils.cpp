#include "utils.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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
