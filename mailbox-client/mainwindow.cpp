#include "mainwindow.h"
#include "ui_mainwindow.h"

#include    <QFileDialog>
#include    <QCloseEvent>
#include <QTimer>

void MainWindow::connections()
{
    //connect(curNewSocket, SIGNAL(transferBoxNames(QStringList)), this, SLOT(onTransferBoxNames(QStringList)));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();  //关闭所有子窗口
    event->accept();
}

void MainWindow::iniTree() {
    ui->treeWidget->clear();

    //ui->treeWidget->expandAll(); // 默认展开所有项

    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setHeaderHidden(true);

    // 添加根节点
    QTreeWidgetItem* treeRootItem = new QTreeWidgetItem(ui->treeWidget);
    treeRootItem->setText(0, curNewSocket->getId());
    //rootItem->setIcon(0, QIcon(":/icons/folder.png"));
    treeRootItems.insert(curNewSocket->getId(), treeRootItem);

    // 添加子节点
    //QTreeWidgetItem* childItem1 = new QTreeWidgetItem(rootItem);
    //childItem1->setText(0, "收件箱");
    //childItem1->setIcon(0, QIcon(":/icons/star.png"));

    //QTreeWidgetItem* childItem2 = new QTreeWidgetItem(rootItem);
    //childItem2->setText(0, "草稿箱");
    //childItem2->setIcon(0, QIcon(":/icons/tag.png"));
}

MainWindow::MainWindow(NewSocket *newSocket, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    curNewSocket(newSocket)
{
    ui->setupUi(this);

    this->setWindowTitle("Client");

    //setCentralWidget(ui->mdiArea);
    setWindowState(Qt::WindowMaximized);  //窗口最大化显示
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
//    this->setAutoFillBackground(true);

    id = curNewSocket->getId();

    // 初始化 QTreeWidget
    iniTree();

    connections();

    labStatus = new QLabel(this);
    labStatus->setMidLineWidth(200);
    ui->statusBar->addWidget(labStatus);
    //popup = new QDialog(this, Qt::Popup | Qt::FramelessWindowHint);

    ui->treeWidget->setMaximumWidth(280);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actDoc_Open_triggered()
{//打开文件
//必须先获取当前MDI子窗口，再使用打开文件对话框，否则无法获得活动的子窗口
    bool needNew=false;     // 是否需要新建子窗口

    TFormDoc    *formDoc;
    if (ui->mdiArea->subWindowList().size()>0) //如果有打开的主窗口，获取活动窗口
    {
        formDoc=(TFormDoc*)ui->mdiArea->activeSubWindow()->widget();
        needNew=formDoc->isFileOpened();        //文件已经打开，需要新建窗口
    }
    else
        needNew=true;

    QString curPath=QDir::currentPath();
    QString aFileName=QFileDialog::getOpenFileName(this,tr("打开一个文件"),curPath,
                 "C程序文件(*.h *cpp);;文本文件(*.txt);;所有文件(*.*)");
    if (aFileName.isEmpty())
        return;     //如果未选择文件，退出

    if (needNew)    //需要新建子窗口
    {
        formDoc = new TFormDoc(curNewSocket, this);   //指定父窗口，必须在父窗口为Widget窗口提供一个显示区域
        ui->mdiArea->addSubWindow(formDoc);
    }

    formDoc->loadFromFile(aFileName);   //打开文件
    formDoc->show();

    ui->actCut->setEnabled(true);
    ui->actCopy->setEnabled(true);
    ui->actPaste->setEnabled(true);
    ui->actFont->setEnabled(true);
}


void MainWindow::on_actDoc_New_triggered()
{ //新建文件
    TFormDoc *formDoc = new TFormDoc(curNewSocket, this);
    forms.append(formDoc);
    ui->mdiArea->addSubWindow(formDoc);     //文档窗口添加到MDI
    formDoc->show();    //在单独的窗口中显示

    ui->actCut->setEnabled(true);
    ui->actCopy->setEnabled(true);
    ui->actPaste->setEnabled(true);
    ui->actFont->setEnabled(true);
}

void MainWindow::on_actCut_triggered()
{ //cut
    TFormDoc* formDoc=(TFormDoc*)ui->mdiArea->activeSubWindow()->widget();
    formDoc->textCut();
}

void MainWindow::on_actFont_triggered()
{//设置字体
    TFormDoc* formDoc=(TFormDoc*)ui->mdiArea->activeSubWindow()->widget();
    formDoc->setEditFont();
}

void MainWindow::on_actCopy_triggered()
{//copy
    TFormDoc* formDoc=(TFormDoc*)ui->mdiArea->activeSubWindow()->widget();
    formDoc->textCopy();
}

void MainWindow::on_actPaste_triggered()
{//paste
    TFormDoc* formDoc=(TFormDoc*)ui->mdiArea->activeSubWindow()->widget();
    formDoc->textPaste();
}

void MainWindow::on_mdiArea_subWindowActivated(QMdiSubWindow *arg1)
{//当前活动子窗口切换时
    Q_UNUSED(arg1);
    if (ui->mdiArea->subWindowList().size()==0)
    { //若子窗口个数为零
        ui->actCut->setEnabled(false);
        ui->actCopy->setEnabled(false);
        ui->actPaste->setEnabled(false);
        ui->actFont->setEnabled(false);
        ui->statusBar->clearMessage();
    }
    else
    {
        TFormDoc *formDoc=static_cast<TFormDoc*>(ui->mdiArea->activeSubWindow()->widget());
        ui->statusBar->showMessage(formDoc->currentFileName()); //显示主窗口的文件名
    }
}

void MainWindow::on_actViewMode_triggered(bool checked)
{//MDI 显示模式
    if (checked) //Tab多页显示模式
        ui->mdiArea->setViewMode(QMdiArea::TabbedView); //Tab多页显示模式
    else //子窗口模式
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView); //子窗口模式
    ui->mdiArea->setTabsClosable(checked);  //切换到多页模式下需重新设置
    ui->actCascade->setEnabled(!checked);   //子窗口模式下才有用
    ui->actTile->setEnabled(!checked);
}

void MainWindow::on_actCascade_triggered()
{ //窗口级联展开
    ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_actTile_triggered()
{//平铺展开
    ui->mdiArea->tileSubWindows();
}

void MainWindow::on_actCloseALL_triggered()
{//关闭全部子窗口
    ui->mdiArea->closeAllSubWindows();
}

void MainWindow::on_actDoc_Save_triggered()
{//保存
    TFormDoc* formDoc=(TFormDoc*)ui->mdiArea->activeSubWindow()->widget();
    formDoc->saveToFile();
}

void MainWindow::on_actNewAcc_triggered()
{
    // NewSocket* newSocket = new NewSocket;
    // newSockets.append(newSocket);
}

void MainWindow::on_actPullMails_triggered()
{
    labStatus->setText("正在从远程服务器拉取邮件...");

    // 收取邮件需要连接 imap
    curNewSocket->tryConnection("imap.qq.com");

    QStringList names = curNewSocket->getBoxNames();
    if (names.isEmpty()) {
        qWarning() << "mainwindow: 没有获取到文件夹的名字";
        return;
    }

    QTreeWidgetItem* childItem, *rootItem = treeRootItems.value(curNewSocket->getId());
    // 清除所有子节点
    if (rootItem != nullptr) {
        while (rootItem->childCount() > 0) {
            QTreeWidgetItem* child = rootItem->child(0); // 获取第一个子节点
            rootItem->removeChild(child); // 从 rootItem 中移除该子节点
            delete child; // 删除子节点，释放内存
        }
    }

    for (const QString &name: names) {
        childItem = new QTreeWidgetItem(rootItem);
        childItem->setText(0, name);
        //curNewSocket->getMailsOfBox(mails, name);
    }
    //ui->treeWidget->update();
    ui->treeWidget->expandAll();

    QVector<NewSocket::EmailInfo> mails;
    curNewSocket->getMailsOfBox(mails, "收件箱");  // date subject from to messageId
    curNewSocket->disConnect("imap.qq.com");  // 断开连接

    // 显示
    ui->listWidget->clear();
    for (NewSocket::EmailInfo mail: mails) {
        addItem(mail.date, mail.subject, mail.from);
    }
    // 调整列表的整体样式，添加间隔线
    ui->listWidget->setStyleSheet(
        "QListWidget { "
        "    background-color: white; "
        "    border: none; "
        "} "
        "QListWidget::item { "
        "    border-bottom: 1px solid #dcdcdc; "  // 浅灰色分割线
        "    height: 70px; "
        "    padding: 10px; "  // 增加内边距
        "} "
        "QListWidget::item:selected { "  // 选中项的样式
        "    background-color: #f0f0f0; "  // 选中项背景色
        "    color: black; "
        "} "
        );

    labStatus->setText("拉取邮件完毕");
}

void MainWindow::addItem(QString date, QString subject, QString from) {
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
    QLabel *labFrom = new QLabel(from);
    QLabel *labDate = new QLabel(date);  // "2023-2-24"
    labDate->setStyleSheet("color: gray; margin-left: 5px;");  // 为日期设置样式和间隔

    topLayout->addWidget(topIcon);  // 添加图标到布局
    topLayout->addSpacing(15);  // 增加图标与文本之间的横向间隔
    topLayout->addWidget(labFrom);
    topLayout->addStretch(1);  // 添加弹性空间
    topLayout->addWidget(labDate);

    // 创建第二行的布局和内容，只有描述文本，确保与第一行对齐
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    QLabel *bottomIcon = new QLabel;
    bottomIcon->setPixmap(QPixmap("C:\\Users\\21863\\Pictures\\v2-d9f41bc31aa52513b1e8a7af6d9ecd16_1440w.jpg").scaled(16, 16));  // 设置第二行的图标
    QLabel *labSubj = new QLabel(subject);
    //description->setStyleSheet("margin-left: 5px;");  // 设置文本的左边距，确保与上面标题对齐
    QLabel *temp = new QLabel;

    bottomLayout->addWidget(bottomIcon);  // 添加图标到布局
    bottomLayout->addSpacing(15);
    bottomLayout->addWidget(labSubj);
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(temp);

    // 将行布局添加到主 Widget
    widgetLayout->addLayout(topLayout);
    widgetLayout->addLayout(bottomLayout);

    // 将自定义 widget 设置到列表项
    ui->listWidget->setItemWidget(item, widget);
}

void MainWindow::on_listWidget_itemActivated(QListWidgetItem *item)
{
    on_actDoc_New_triggered();
    if (forms.empty()) {
        qWarning() << "创建新小窗失败";
        return;
    }
    TFormDoc *form = forms.last();
    int row = ui->listWidget->row(item);
    form->setText(curNewSocket->getMailDetail(row));
}

