#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSslSocket>
#include <QTreeView>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QApplication>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
void MainWindow::list() {
    ui->listWidget->clear();  // 清除列表中的所有项

    // 创建一个新的 QListWidgetItem
    QListWidgetItem *item = new QListWidgetItem();
    ui->listWidget->addItem(item);  // 添加项到 listWidget

    // 创建自定义 widget 用于显示两行文本及图标
    QWidget *widget = new QWidget;
    QVBoxLayout *widgetLayout = new QVBoxLayout(widget);
    widgetLayout->setSpacing(0);  // 设置布局内部的间距
    widgetLayout->setContentsMargins(0, 0, 0, 0);  // 设置布局的边距

    // 创建第一行的布局和内容，包括图标、标题和日期
    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *topIcon = new QLabel;
    topIcon->setPixmap(QPixmap("C:\\Users\\21863\\Pictures\\v2-d9f41bc31aa52513b1e8a7af6d9ecd16_1440w.jpg").scaled(16, 16));  // 设置图标
    QLabel *title = new QLabel("望舒");
    QLabel *date = new QLabel("2023-2-24");
    date->setStyleSheet("color: gray; margin-left: 5px;");  // 为日期设置样式和间隔

    topLayout->addWidget(topIcon);  // 添加图标到布局
    topLayout->addSpacing(15);  // 增加图标与文本之间的横向间隔
    topLayout->addWidget(title);
    topLayout->addStretch(1);  // 添加弹性空间
    topLayout->addWidget(date);

    // 创建第二行的布局和内容，只有描述文本，确保与第一行对齐
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    QLabel *bottomIcon = new QLabel;
    bottomIcon->setPixmap(QPixmap("C:\\Users\\21863\\Pictures\\v2-d9f41bc31aa52513b1e8a7af6d9ecd16_1440w.jpg").scaled(16, 16));  // 设置第二行的图标
    QLabel *description = new QLabel("Fw: Steema Software LICENSE INFORMA...");
    //description->setStyleSheet("margin-left: 5px;");  // 设置文本的左边距，确保与上面标题对齐
    QLabel *temp = new QLabel;

    bottomLayout->addWidget(bottomIcon);  // 添加图标到布局
    bottomLayout->addSpacing(15);
    bottomLayout->addWidget(description);
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(temp);


    // 将行布局添加到主 Widget
    widgetLayout->addLayout(topLayout);
    widgetLayout->addLayout(bottomLayout);

    // 将自定义 widget 设置到列表项
    ui->listWidget->setItemWidget(item, widget);

    // 可选：调整列表的整体样式
    ui->listWidget->setStyleSheet("QListWidget { background-color: white; border: none; }"
                                  "QListWidget::item { height: 70px; }");  // 设置项的高度和背景
}

// void MainWindow::list() {
//     ui->listWidget->clear();  // 清除列表中的所有项

//     // 创建一个新的 QListWidgetItem
//     QListWidgetItem *item = new QListWidgetItem();
//     ui->listWidget->addItem(item);  // 添加项到 listWidget

//     // 创建自定义 widget 用于显示两行文本及图标
//     QWidget *widget = new QWidget;
//     QVBoxLayout *widgetLayout = new QVBoxLayout(widget);

//     // 创建第一行的布局和内容，包括图标、文本和日期
//     QHBoxLayout *topLayout = new QHBoxLayout();
//     QLabel *topIcon = new QLabel;
//     topIcon->setPixmap(QPixmap("C:\\Users\\21863\\Pictures\\v2-d9f41bc31aa52513b1e8a7af6d9ecd16_1440w.jpg").scaled(16, 16));  // 设置图标
//     QLabel *title = new QLabel("望舒");
//     QLabel *date = new QLabel("2023-2-24");
//     date->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

//     topLayout->addWidget(topIcon);  // 添加图标到布局
//     topLayout->addWidget(title);
//     topLayout->addStretch();  // 添加弹性空间，使日期显示在最右侧
//     topLayout->addWidget(date);

//     // 创建第二行的布局和内容，包括图标和描述
//     QHBoxLayout *bottomLayout = new QHBoxLayout();
//     QLabel *bottomIcon = new QLabel;
//     bottomIcon->setPixmap(QPixmap("C:\\Users\\21863\\Pictures\\v2-d9f41bc31aa52513b1e8a7af6d9ecd16_1440w.jpg").scaled(16, 16));  // 设置第二行的图标
//     QLabel *description = new QLabel("Fw: Steema Software LICENSE INFORMA...");

//     bottomLayout->addWidget(bottomIcon);  // 添加图标到布局
//     bottomLayout->addWidget(description);

//     // 将行布局添加到主 Widget
//     widgetLayout->addLayout(topLayout);
//     widgetLayout->addLayout(bottomLayout);
//     widgetLayout->setSpacing(2);  // 设置布局内部的间距
//     widgetLayout->setContentsMargins(5, 5, 5, 5);  // 设置布局的边距

//     // 将自定义 widget 设置到列表项
//     ui->listWidget->setItemWidget(item, widget);

//     // 可选：调整列表的整体样式
//     ui->listWidget->setStyleSheet("QListWidget { background-color: white; border: none; }"
//                                   "QListWidget::item { height: 60px; }");  // 设置项的高度和背景
// }


// void MainWindow::list() {

//         ui->listWidget->clear();  // 清除列表中的所有项

//         // 创建一个新的 QListWidgetItem
//         QListWidgetItem *item = new QListWidgetItem();
//         ui->listWidget->addItem(item);  // 添加项到 listWidget

//         // 创建自定义 widget 用于显示两行文本及图标
//         QWidget *widget = new QWidget;
//         QVBoxLayout *widgetLayout = new QVBoxLayout(widget);

//         // 创建第一行的布局和内容
//         QHBoxLayout *topLayout = new QHBoxLayout();
//         QLabel *icon = new QLabel;
//         icon->setPixmap(QPixmap(":/path/to/icon.png").scaled(16, 16));  // 设置图标
//         QLabel *title = new QLabel("望舒");
//         QLabel *date = new QLabel("2023-2-24");
//         date->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

//         topLayout->addWidget(icon);
//         topLayout->addWidget(title);
//         topLayout->addStretch();  // 添加弹性空间，使日期显示在最右侧
//         topLayout->addWidget(date);

//         // 创建第二行的文本
//         QLabel *description = new QLabel("Fw: Steema Software LICENSE INFORMA...");

//         // 添加布局到widget
//         widgetLayout->addLayout(topLayout);
//         widgetLayout->addWidget(description);
//         widgetLayout->setSpacing(2);  // 设置布局内部的间距
//         widgetLayout->setContentsMargins(5, 0, 5, 0);  // 设置布局的边距

//         // 将自定义 widget 设置到列表项
//         ui->listWidget->setItemWidget(item, widget);

//         // 可选：调整列表的整体样式
//         ui->listWidget->setStyleSheet("QListWidget { background-color: white; border: none; }"
//                                       "QListWidget::item { height: 60px; }");  // 设置项的高度和背景




//     // ui->listWidget->clear();  // 清除列表中的所有项

//     // // 创建一个新的 QListWidgetItem
//     // QListWidgetItem *item = new QListWidgetItem();
//     // ui->listWidget->addItem(item);  // 添加项到 listWidget

//     // // 创建自定义 widget 用于显示两行文本及图标
//     // QWidget *widget = new QWidget;
//     // QVBoxLayout *widgetLayout = new QVBoxLayout(widget);

//     // // 创建第一行（标题行）的布局和内容
//     // QHBoxLayout *titleLayout = new QHBoxLayout();
//     // QLabel *titleIcon = new QLabel;
//     // // QIcon("C:\\Users\\21863\\Pictures\\v2-d9f41bc31aa52513b1e8a7af6d9ecd16_1440w.jpg"
//     // titleIcon->setPixmap(QPixmap("C:\\Users\\21863\\Pictures\\v2-d9f41bc31aa52513b1e8a7af6d9ecd16_1440w.jpg").scaled(16, 16));
//     // QLabel *titleText = new QLabel("Fw: Steema Software LICENSE INFORMA...");
//     // titleLayout->addWidget(titleIcon);
//     // titleLayout->addWidget(titleText);
//     // widgetLayout->addLayout(titleLayout);

//     // // 创建第二行（日期行）的布局和内容
//     // QHBoxLayout *dateLayout = new QHBoxLayout();
//     // QLabel *dateIcon = new QLabel;
//     // dateIcon->setPixmap(QPixmap("C:\\Users\\21863\\Pictures\\v2-d9f41bc31aa52513b1e8a7af6d9ecd16_1440w.jpg").scaled(16, 16));
//     // QLabel *dateText = new QLabel("2023-2-24");
//     // dateLayout->addWidget(dateIcon);
//     // dateLayout->addWidget(dateText);
//     // widgetLayout->addLayout(dateLayout);

//     // // 设置内边距和间距
//     // widgetLayout->setSpacing(0);  // 设置布局内部的间距
//     // widgetLayout->setContentsMargins(0, 0, 0, 0);  // 设置布局的边距

//     // // 将自定义 widget 设置到列表项
//     // ui->listWidget->setItemWidget(item, widget);

//     // // 可选：调整列表的整体样式
//     // ui->listWidget->setStyleSheet("QListWidget { background-color: white; border: none; }"
//     //                               "QListWidget::item { height: 50px; }");  // 设置项的高度和背景
// }


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tree1();
    tree2();
    tree3();

    list();

    // 展示窗口
    //ui->treeView_2->show();

    // connection();
    // QString msg = "";
    // if (socket->waitForReadyRead()) {
    //     msg = socket->readAll();
    //     qDebug() << "===========================================";
    //     //qDebug() << msg;
    // }

    // const QString complexRegexStr = R"raw(\* \d+ FETCH \(ENVELOPE \("([^"]+)" "([^"]+)" )raw"
    //                                 R"raw(\((?:[^()]+|\([^()]*\))*\) )raw"  // 处理可能的嵌套括号
    //                                 R"raw(\((?:[^()]+|\([^()]*\))*\) )raw"
    //                                 R"raw(\((?:[^()]+|\([^()]*\))*\) )raw"
    //                                 R"raw(\((?:[^()]+|\([^()]*\))*\) )raw"
    //                                 R"raw(NIL NIL NIL ""\)\))raw";
    // QRegularExpression complexRegex(complexRegexStr);



    // QStringList lines = msg.split("\r\n", Qt::SkipEmptyParts);

    // int cnt = 0;
    // for (const QString &line : lines) {
    //     qDebug() << "-- 服务器：" << line;

    //     QRegularExpressionMatch match = complexRegex.match(line);
    //     if (match.hasMatch()) {
    //         EmailInfo info;
    //         info.date = match.captured(1);
    //         info.subject = decodeBase64(match.captured(2));
    //         info.from = parseAddress(match.captured(3));
    //         info.to = parseAddress(match.captured(4));
    //         // Assuming CC and BCC are not provided in this data
    //         info.messageId = match.captured(10);

    //         //emails.append(info);

    //         cnt++;
    //         qDebug() << "第 " << cnt << " 封邮件在 " << info.date << " 发送自 " << info.from << " ，主题是 " << info.subject;
    //     } else {
    //         qWarning() << "No match found";
    //     }
    // }

}

QString MainWindow::decodeBase64(const QString &base64EncodedText) {
    // 从 Base64 编码的 QString 创建 QByteArray
    QByteArray decodedByteArray = QByteArray::fromBase64(base64EncodedText.toUtf8());

    // 将 QByteArray 转换回 QString（假设原始文本是 UTF-8 编码的）
    return QString::fromUtf8(decodedByteArray);
}

QString MainWindow::parseAddress(const QString &address) {
    // 正确处理转义和字符串格式
    QRegularExpression addressRegex(R"(\"([^"]*)\" NIL \"([^"]*)\" \"([^"]*)\")");
    QRegularExpressionMatch match = addressRegex.match(address);
    if (match.hasMatch()) {
        return QString("%1@%2").arg(match.captured(2), match.captured(3));
    }
    return QString();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connection()
{
    socket = new QSslSocket;
    socket->connectToHostEncrypted("imap.qq.com", 993);
    qDebug() << "连接服务器...";
    // 等待连接成功
    if (!socket->waitForConnected()) {
        qDebug() << "连接失败:" << socket->errorString();
        return;
    }
    if (socket->waitForReadyRead()) {
        qDebug() << socket->readAll();
    }

    QString command = "A1 LOGIN 3445003795@qq.com lddvdbkivnapchcb\r\n";
    socket->write(command.toUtf8());
    qDebug() << command;
    if (socket->waitForReadyRead()) {
        qDebug() << socket->readAll();
    }

    command = "a1 LIST \"\" \"*\"\r\n";
    socket->write(command.toUtf8());
    qDebug() << command;
    if (socket->waitForReadyRead()) {
        qDebug() << socket->readAll();
    }

    socket->waitForReadyRead();
    qDebug() << socket->readAll();

    command = "2 SELECT INBOX\r\n";
    socket->write(command.toUtf8());
    qDebug() << command;
    if (socket->waitForReadyRead()) {
        qDebug() << socket->readAll();
    }

    command = "3 FETCH 1:16 (ENVELOPE)\r\n";
    socket->write(command.toUtf8());
    qDebug() << command;
}

void MainWindow::tree1()
{
    // 创建模型
    QStandardItemModel *model = new QStandardItemModel();
    //QTreeView *treeView = new QTreeView();

    // 设置模型的列标题
    model->setHorizontalHeaderLabels({"Sender", "Subject", "Date"});

    // 添加一些条目
    QList<QStandardItem *> firstItem;
    firstItem << new QStandardItem("xhb007@sina.com");
    firstItem << new QStandardItem("Your DevExpress Account Information");
    firstItem << new QStandardItem("2023-2-24");
    model->appendRow(firstItem);

    QList<QStandardItem *> secondItem;
    secondItem << new QStandardItem("Nitesh from B12");
    secondItem << new QStandardItem("Best way to turn leads into clients");
    secondItem << new QStandardItem("10:35");
    model->appendRow(secondItem);

    QList<QStandardItem *> thirdItem;
    thirdItem << new QStandardItem("Apple");
    thirdItem << new QStandardItem("Your monthly report is available");
    thirdItem << new QStandardItem("Yesterday");
    model->appendRow(thirdItem);

    // 设置模型
    ui->treeView->setModel(model);

    // 展开所有项，设置列宽适应内容
    ui->treeView->expandAll();
    for (int column = 0; column < model->columnCount(); ++column) {
        ui->treeView->resizeColumnToContents(column);
    }
}

void MainWindow::tree2()
{
    // 创建模型
    QStandardItemModel *model1 = new QStandardItemModel();
    //QTreeView *treeView = new QTreeView();

    // 设置模型的根节点标题
    model1->setHorizontalHeaderLabels({"Name", "Description"});

    // 创建一些条目
    QStandardItem *rootNode = model1->invisibleRootItem();

    // 第一个顶级节点
    QStandardItem *node1 = new QStandardItem("Node 1");
    QStandardItem *description1 = new QStandardItem("Description of Node 1");
    rootNode->appendRow({node1, description1});

    // 第一个顶级节点的子节点
    QStandardItem *child1 = new QStandardItem("Child 1");
    QStandardItem *childDesc1 = new QStandardItem("Description of Child 1");
    node1->appendRow({child1, childDesc1});

    QStandardItem *child2 = new QStandardItem("Child 2");
    QStandardItem *childDesc2 = new QStandardItem("Description of Child 2");
    node1->appendRow({child2, childDesc2});

    // 第二个顶级节点
    QStandardItem *node2 = new QStandardItem("Node 2");
    QStandardItem *description2 = new QStandardItem("Description of Node 2");
    rootNode->appendRow({node2, description2});

    // 设置模型
    ui->treeView_2->setModel(model1);

    // 展开所有项
    ui->treeView_2->expandAll();

    // 设置列宽自适应内容
    ui->treeView_2->resizeColumnToContents(0);
    ui->treeView_2->resizeColumnToContents(1);
}

void MainWindow::tree3()
{
    // Step 1: Setup database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("D:\\codeField\\mailbox\\database\\user1.db"); // 设置数据库文件的路径

    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", "Unable to connect to the database!");
        return;
    }

    // Step 2: Setup the model
    QSqlTableModel *model = new QSqlTableModel();
    model->setTable("inbox");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit); // 设置编辑策略
    model->select(); // 加载数据

    // 设置列标题（可选）
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("date"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("subject"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("from"));

    // Step 3: Setup the QTreeView
    //QTreeView *treeView = new QTreeView();
    ui->treeView_3->setModel(model);
    ui->treeView_3->hideColumn(0); // 隐藏ID列，如果不需要显示
    ui->treeView_3->show();
}
