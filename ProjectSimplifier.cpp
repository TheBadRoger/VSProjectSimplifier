#include "ProjectSimplifier.h"
#include <QMessageBox>
#include <QTreeWidgetItem>

ProjectSimplifier::ProjectSimplifier(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

ProjectSimplifier::~ProjectSimplifier()
{}

void ProjectSimplifier::executeSimpifyDirect() {
    ui.LAB_ShowTips->setText("");
    ui.BTN_OpenDir->setEnabled(false);
    if (CheckValidDirect()) DeleteFile(RootPath);
    ui.LAB_DeletingDirStatus->setText("完成");
    ui.LAB_DeletingFileStatus->setText("");
    ui.BTN_OpenDir->setEnabled(true);
}

void ProjectSimplifier::DeleteFile(const QString& localpath) {
    QFileInfo local(localpath);
    if (local.isDir()) {
        QDir localdir(localpath, "*", DefaultSort, DefaultFilter);

        ui.LAB_DeletingDirStatus->setText(QString("正在删除目录：%1 ...").arg(localpath));
        for (const auto& sitem : localdir.entryInfoList()) DeleteFile(sitem.filePath());
        if (!localdir.count()) localdir.remove(localpath);
    }
    else if (local.isFile() && isRemovableFile(local.suffix())) {
        QFile localfile(localpath);

        ui.LAB_DeletingFileStatus->setText(QString("正在删除文件：%1 ...").arg(localfile.fileName()));
        localfile.setPermissions(localpath, QFile::WriteOwner);
        localfile.remove(localpath);
    }
}

void ProjectSimplifier::GetDirect() {
    RootPath = QFileDialog::getExistingDirectory(this, "选择目录", QCoreApplication::applicationDirPath());
    ui.BOX_InputFilePath->setText(RootPath);
    if (!ui.BOX_InputFilePath->text().isEmpty() && CheckValidDirect()) ShowDirectOnBox();
}

void ProjectSimplifier::ShowDirectOnBox() {

    ClearData();
    ui.BTN_OpenDir->setEnabled(false);

    GeneratePrimitiveTree(RootPath, CountCatch, PrimitiveSize);
    ui.BOX_PrimitiveDirectView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.LAB_PrimitiveSize->setText(AutoShowSize(PrimitiveSize));

    GenerateProcessedTree(RootPath, CountCatch, ProcessedSize);
    ui.BOX_ProcessedDirectView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.LAB_ProcessedSize->setText(AutoShowSize(ProcessedSize));

    ui.BTN_executeProcess->setEnabled(true);
    ui.BTN_OpenDir->setEnabled(true);
    ui.LAB_ShowRemovableSize->setText(QString("- %1\n→").arg(AutoShowSize(PrimitiveSize - ProcessedSize)));
    ui.LAB_ShowTips->setText(QString("<b>粗体字</b>代表处理后会被删除的文件"));
}

void ProjectSimplifier::initalizeUI() {
    ui.LAB_ShowRemovableSize->setText("");
    ui.LAB_DeletingDirStatus->setText("");
    ui.LAB_DeletingFileStatus->setText("");
    ui.LAB_ShowTips->setText("");
}

bool ProjectSimplifier::CheckValidDirect() {
    if (!RootPath.isEmpty()) {
        QFileInfo CheckDir(RootPath);
        if (!CheckDir.exists() || !CheckDir.isDir()) {
            QMessageBox::critical(this, "错误", QString("目录 %1 不存在或不合法，请重新选择目录").arg(RootPath), QMessageBox::Ok);
            return false;
        }
        else return true;
    }
    else QMessageBox::critical(this, "错误", QString("请先选择目录"), QMessageBox::Ok);
}

void ProjectSimplifier::ClearData() {
    QTreeWidgetItem* item = ui.BOX_PrimitiveDirectView->topLevelItem(0);
    while (item) {
        ClearTree(item);
        item = ui.BOX_PrimitiveDirectView->topLevelItem(0);
    }

    item = ui.BOX_ProcessedDirectView->topLevelItem(0);
    while (item) {
        ClearTree(item);
        item = ui.BOX_ProcessedDirectView->topLevelItem(0);
    }

    PrimitiveSize = ProcessedSize = 0;
}

bool ProjectSimplifier::isRemovableFile(const QString& fext) {
    if (extList.find(fext) == extList.end())return false;
    else return true;
}

void ProjectSimplifier::ClearTree(QTreeWidgetItem* item) {
    if (!item->childCount()) delete item;
    else {
        for (int i = 0; i < item->childCount(); i++){
            QTreeWidgetItem* childItem = item->child(0);
            ClearTree(childItem);
        }
        delete item;
    }
}

void ProjectSimplifier::GeneratePrimitiveTree(const QString& localpath, qint64& scount, qint64& fsize, QTreeWidgetItem* parent) {
    QDir local(localpath);
    local.setFilter(DefaultFilter);
    for (const auto& item : local.entryInfoList(QDir::NoFilter, DefaultSort)) {
        if (item.isDir()) {
            QTreeWidgetItem* diritem = new QTreeWidgetItem(QStringList() << item.fileName(), (int)itemType::Dir);
            qint64 subItemCount = QDir(item.filePath(), "*", DefaultSort, DefaultFilter).count(), localSize = 0;

            GeneratePrimitiveTree(item.filePath(), subItemCount, localSize, diritem);

            fsize += localSize;
            diritem->setText(1, AutoShowSize(localSize));
            if (!subItemCount) SetItemBold(diritem), scount--;

            insertTreeItem(ui.BOX_PrimitiveDirectView, diritem, parent);
        }
        else if (item.isFile()) {
            QTreeWidgetItem* fileitem = new QTreeWidgetItem(QStringList() << item.fileName() << AutoShowSize(item.size()), (int)itemType::File);

            if (isRemovableFile(item.suffix())) SetItemBold(fileitem), scount--;
            fsize += item.size();

            insertTreeItem(ui.BOX_PrimitiveDirectView, fileitem, parent);
        }
    }
}

void ProjectSimplifier::GenerateProcessedTree(const QString& localpath, qint64& scount, qint64& fsize, QTreeWidgetItem* parent) {
    QDir local(localpath);
    local.setFilter(DefaultFilter);
    for (const auto& item : local.entryInfoList(QDir::NoFilter, QDir::DirsFirst | QDir::Name)) {
        if (item.isDir()) {
            QTreeWidgetItem* diritem = new QTreeWidgetItem(QStringList() << item.fileName(), (int)itemType::Dir);
            qint64 subItemCount = QDir(item.filePath(), "*", DefaultSort, DefaultFilter).count(), localSize = 0;

            GenerateProcessedTree(item.filePath(), subItemCount, localSize, diritem);

            diritem->setText(1, AutoShowSize(localSize));

            if (subItemCount) insertTreeItem(ui.BOX_ProcessedDirectView, diritem, parent), fsize += localSize;
            else scount--;

        }
        else if (item.isFile()) {
            QTreeWidgetItem* fileitem = new QTreeWidgetItem(QStringList() << item.fileName() << AutoShowSize(item.size()), (int)itemType::File);

            if (!isRemovableFile(item.suffix())) insertTreeItem(ui.BOX_ProcessedDirectView, fileitem, parent), fsize += item.size();
            else scount--;
        }
    }
}

double ProjectSimplifier::ShowSize(qint64 byte, SizeUnits unit) {
    switch (unit) {
    case SizeUnits::Byte:
        return byte;
        break;

    case SizeUnits::Kilo:
        return byte / 1024.0;
        break;

    case SizeUnits::Mega:
        return byte / 1048576.0;
        break;

    case SizeUnits::Giga:
        return byte / 1073741824.0;
        break;

    default:
        return byte;
    }
}

QString ProjectSimplifier::AutoShowSize(qint64 byte) {
    if (byte <= 1024) return QString("%1 B").arg(ShowSize(byte, SizeUnits::Byte));
    else if (byte <= 1048576) return QString("%1 KB").arg(QString::number(ShowSize(byte, SizeUnits::Kilo), 'f', 2));
    else if (byte <= 1073741824) return QString("%1 MB").arg(QString::number(ShowSize(byte, SizeUnits::Mega), 'f', 2));
    else return QString("%1 GB").arg(QString::number(ShowSize(byte, SizeUnits::Giga), 'f', 2));
}

void ProjectSimplifier::SetItemBold(QTreeWidgetItem* item) {
    QFont bold = item->font(0);
    bold.setBold(true);
    for (int i = 0; i <= item->columnCount() - 1; i++)item->setFont(i, bold);
}

void ProjectSimplifier::insertTreeItem(QTreeWidget* ptree, QTreeWidgetItem* item, QTreeWidgetItem* parent) {
    if (parent == nullptr) ptree->addTopLevelItem(item);
    else parent->addChild(item);
}
