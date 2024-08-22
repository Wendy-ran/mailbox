#include "mainwindow.h"

#include <QApplication>
#include <QByteArray>
#include <QDebug>

QString decodeMimeText(const QString &encodedText) {
    // Remove any whitespace between MIME encoded words (RFC 2047)
    QString compact = encodedText.simplified().replace(" =?", "=?");

    // Split the encoded words
    QStringList encodedWords = compact.split("?=");
    QString decodedText;
    for (const QString &word : encodedWords) {
        if (word.trimmed().isEmpty()) continue;
        // Decode each part
        auto parts = word.split('?');
        if (parts.count() != 5) continue;
        QByteArray data = QByteArray::fromBase64(parts.at(3).toLatin1());
        decodedText += QString::fromUtf8(data);
    }
    return decodedText;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString fromEncoded = "=?utf-8?B?UVHpgq7nrrHlm6LpmJ8=?=";
    QString subjectEncoded = "=?utf-8?B?IkZveG1haWwi5bey6I635b6X5LqG5L2g55qEUVE=?= "
                             "=?utf-8?B?6YKu566x6LSm5Y+355qE6K6/6Zeu5p2D6ZmQ?=";

    QString from = decodeMimeText(fromEncoded);
    QString subject = decodeMimeText(subjectEncoded);

    qDebug() << "From:" << from;
    qDebug() << "Subject:" << subject;

    MainWindow w;
    w.show();
    return a.exec();
}


