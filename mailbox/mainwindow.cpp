#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include    <QPainter>
#include    <QFileDialog>
#include    <QCloseEvent>
#include    "tformdoc.h"

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

    //setCentralWidget(ui->mdiArea);
    setWindowState(Qt::WindowMaximized);  //窗口最大化显示
    ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
//    this->setAutoFillBackground(true);

    id = curNewSocket->getId();
    //id = "3445003795";
    //code = "lddvdbkivnapchcb";

    // 初始化 QTreeWidget
    iniTree();

    connections();
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
    // on_actPullMails_triggered(); 手动调用这个槽函数

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

        // 遍历每个 BOX 收取邮件
        // if (name == "收件箱") {
        //     //curNewSocket->getMails(name);

        // }

    }
    //ui->treeWidget->update();
    ui->treeWidget->expandAll();

    NewSocket::EmailInfo mail;
    for (int i = 1; i <= 14; i++) {
        if (!curNewSocket->getMailofBox(i, mail, "收件箱")) {
            qWarning() << "获取邮件失败";
        }
    }


}

