#include "mainwindow.h"
#include <QApplication>
#include    "tdialoglogin.h"
#include <QTcpSocket>
#include "newsocket.h"
#include <QMessageBox>
#include "objectpool.h"
// #include <QSslSocket>
// #include <QNetworkAccessManager>
// #include <vmime/vmime.hpp>
//#include <iostream>
// #include <cryptlib.h>
// #include <osrng.h>
// #include <openssl/aes.h>
// #include <openssl/rand.h>
//#include <cstring>

// #include <vmime/vmime.hpp>
// #include <vmime/platforms/posix/posixHandler.hpp>

// int encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key,
//             unsigned char* iv, unsigned char* ciphertext) {
//     AES_KEY enc_key;
//     AES_set_encrypt_key(key, 256, &enc_key);
//     AES_cbc_encrypt(plaintext, ciphertext, plaintext_len, &enc_key, iv, AES_ENCRYPT);
//     return 0;
// }

// int decrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* key,
//             unsigned char* iv, unsigned char* plaintext) {
//     AES_KEY dec_key;
//     AES_set_decrypt_key(key, 256, &dec_key);
//     AES_cbc_encrypt(ciphertext, plaintext, ciphertext_len, &dec_key, iv, AES_DECRYPT);
//     return 0;
// }

// void fetchEmailsFromQQ() {
//     try {
//         // 创建 vmime 的 session
//         vmime::shared_ptr<vmime::net::session> sess = vmime::make_shared<vmime::net::session>();

//         // 设置连接 URL
//         vmime::utility::url url("imaps://imap.qq.com:993");
//         vmime::shared_ptr<vmime::net::store> store = sess->getStore(url);
//         store->setProperty("options.need-authentication", true);
//         store->setProperty("auth.username", "2186362422@qq.com");
//         store->setProperty("auth.password", "lddvdbkivnapchcb");
//         store->setProperty("connection.tls", true);  // 使用 TLS

//         // 连接到邮箱服务器
//         store->connect();

//         // 使用 vmime::utility::path 创建路径
//         vmime::utility::path folderPath;
//         folderPath.appendComponent(vmime::word("INBOX"));

//         // 打开 INBOX 文件夹
//         vmime::shared_ptr<vmime::net::folder> folder = store->getDefaultFolder()->getFolder(folderPath);
//         folder->open(vmime::net::folder::MODE_READ_ONLY);

//         // 获取邮件数量
//         vmime::size_t count = folder->getMessageCount();
//         std::vector<vmime::shared_ptr<vmime::net::message>> messages = folder->getMessages(vmime::net::messageSet::byNumber(1, count));

//         // 拉取邮件信息
//         folder->fetchMessages(messages, vmime::net::fetchAttributes::ENVELOPE);
//         for (const auto& msg : messages) {
//             vmime::shared_ptr<const vmime::header> hdr = msg->getHeader();
//             std::cout << "Subject: " << hdr->Subject()->getValue<vmime::string>() << std::endl;
//         }

//         // 关闭文件夹和断开连接
//         folder->close(false);
//         store->disconnect();
//     } catch (const vmime::exception& e) {
//         std::cerr << "VMime exception: " << e.what() << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "Standard exception: " << e.what() << std::endl;
//     }
// }

// bool isSSL() {
//     qDebug()<< "ssl: " << QSslSocket::sslLibraryBuildVersionString();
//     qDebug() << "SSL library Build Version String:" << QSslSocket::sslLibraryBuildVersionString();
//     unsigned char key[AES_BLOCK_SIZE]; // AES_BLOCK_SIZE = 16
//     unsigned char iv[AES_BLOCK_SIZE];
//     unsigned char plaintext[128] = "Hello, world! This is a test message.";
//     unsigned char ciphertext[128];
//     unsigned char decryptedtext[128];

//     // Generate random key and IV
//     if (!RAND_bytes(key, sizeof(key)) || !RAND_bytes(iv, sizeof(iv))) {
//         std::cerr << "Error generating random bytes.\n";
//         return false;
//     }

//     std::cout << "Plaintext: " << plaintext << std::endl;

//     // Encrypt the plaintext
//     encrypt(plaintext, sizeof(plaintext), key, iv, ciphertext);
//     std::cout << "Ciphertext: ";
//     for (int i = 0; i < sizeof(ciphertext); i++) {
//         printf("%x", ciphertext[i]);
//     }
//     std::cout << std::endl;

//     // Decrypt the ciphertext
//     decrypt(ciphertext, sizeof(ciphertext), key, iv, decryptedtext);
//     std::cout << "Decrypted text: " << decryptedtext << std::endl;
//     return true;
// }

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "unknown";
    const char *function = context.function ? context.function : "unknown";

    switch (type) {
    case QtDebugMsg:
        //fprintf(stderr, "\033[34mDebug: %s (%s:%u, %s)\033[0m\n", localMsg.constData(), file, context.line, function);
        fprintf(stderr, "\x1b[37mDebug: \x1b[34m%s\x1b[37m(%s:%u, %s)\x1b[0m\n", localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        //(stderr, "\033[33mWarning: %s (%s:%u, %s)\033[0m\n", localMsg.constData(), file, context.line, function);
        fprintf(stderr, "\x1b[37mWarning: \033[33m%s\x1b[37m(%s:%u, %s)\x1b[0m\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        //fprintf(stderr, "\033[31mCritical: %s (%s:%u, %s)\033[0m\n", localMsg.constData(), file, context.line, function);
        fprintf(stderr, "\x1b[37mCritical: \033[31m%s\x1b[37m(%s:%u, %s)\x1b[0m\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        //fprintf(stderr, "\033[35mFatal: %s (%s:%u, %s)\033[0m\n", localMsg.constData(), file, context.line, function);
        fprintf(stderr, "\x1b[37mFatal: \033[35m%s\x1b[37m(%s:%u, %s)\x1b[0m\n", localMsg.constData(), file, context.line, function);
        abort();
    }
    // fprintf(stderr, "\033[1;31m这是红色加粗的文本\033[0m\n");  \033[颜色代码m
    // 这里的 \033[1;31m 设置文本为加粗和红色，\033[0m 用于重置样式，以防后续文本也被同样格式化
    // fprintf(stderr, "\x1b[31m这是红色的部分\x1b[0m，\x1b[38;5;247m这是浅灰色的部分\x1b[0m，和 \x1b[34m这是蓝色的部分\x1b[0m。\n");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //isVmime();

    // if (isSSL()) {
    //     qWarning() << "OpenSSL can't work";
    // }

    //fetchEmailsFromQQ();

    qInstallMessageHandler(customMessageHandler);  // 安装消息处理器
    qDebug() << "This is a debug message";
    qWarning() << "This is a warning message";
    qCritical() << "This is a critical message";

    // 使用单例模式的 ObjectPool
    auto& pool = ObjectPool<NewSocket>::getInstance();
    NewSocket* originNewSocket = pool.acquire("originUser");
    //NewSocket *newSocket = new NewSocket;
    bool conFlag = originNewSocket->tryConnection("smtp.qq.com", 5);

    TDialogLogin *dlgLogin = new TDialogLogin(originNewSocket);  //创建Splash对话框
    int ret = dlgLogin->exec();

    if (ret == QDialog::Accepted && conFlag) {
        MainWindow w(originNewSocket);       //显示主窗口
        w.show();
        return a.exec();    //应用程序正常运行
    } else if (!conFlag) {
        QMessageBox::critical(nullptr, "连接错误", "无法连接 QQ 服务器");
        return 0;
    } else {
        //QMessageBox::critical(nullptr, "错误", "程序初始化失败");
        return 0;
    }
}

// QApplication a(argc, argv);
// MainWindow w;
// w.show();
// return a.exec();





