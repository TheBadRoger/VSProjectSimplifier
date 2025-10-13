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
	if (DelFileList.isEmpty())
        ui.statusBar->showMessage(QString("没有可删除的文件"));
    else {
        for (const auto& item : DelFileList) {
            if (QFileInfo(item).isDir())
                QDir(item).removeRecursively();

            else if (QFileInfo(item).isFile())
                QFile(item).remove();

            ui.statusBar->showMessage(QString("正在删除：%1").arg(item));
        }
        ui.statusBar->showMessage(QString("完成"));
    }
}

void ProjectSimplifier::GetDirect() {
    QString tmpPath = QFileDialog::getExistingDirectory(this, "选择目录", RootPath);
    if (!tmpPath.isEmpty()) {
        RootPath = tmpPath;
        ui.BOX_InputFilePath->setText(RootPath);
        ShowDirectOnBox();
    }
}

void ProjectSimplifier::ShowDirectOnBox() {

    ClearData();
    ui.BTN_OpenDir->setEnabled(false);

    qint64 PlaceHolder = 0;
    GenerateDirTree(RootPath, PlaceHolder, PrimitiveSize, ProcessedSize);

    ui.BOX_PrimitiveDirectView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.LAB_PrimitiveSize->setText(AutoShowSize(PrimitiveSize));
    ui.BOX_ProcessedDirectView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.LAB_ProcessedSize->setText(AutoShowSize(ProcessedSize));

    ui.BTN_executeProcess->setEnabled(true);
    ui.BTN_OpenDir->setEnabled(true);
    ui.LAB_ShowRemovableSize->setText(QString("- %1\n→").arg(AutoShowSize(PrimitiveSize - ProcessedSize)));
    ui.LAB_ShowTips->setText(QString("<b>粗体字</b>代表处理后会被删除的文件"));
}

void ProjectSimplifier::initalizeUI() {
    ui.LAB_ShowRemovableSize->setText("");
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
	ui.BOX_PrimitiveDirectView->clear();
    ui.BOX_ProcessedDirectView->clear();
    PrimitiveSize = ProcessedSize = 0;
}

bool ProjectSimplifier::isRemovableFile(const QString& fext) {
    if (extList.find(fext) == extList.end()) return false;
    else return true;
}

void ProjectSimplifier::GenerateDirTree(const QDir& local, qint64& scount, qint64& primsize, qint64& procsize, QTreeWidgetItem* primparent, QTreeWidgetItem* procparent) {
    for (const auto& item : local.entryInfoList(DefaultFilter, DefaultSort)) {
        ui.statusBar->showMessage(QString("正在检查：%1").arg(item.path()));

        if (item.isDir()) {
            QTreeWidgetItem* diritem_prim = new QTreeWidgetItem(QStringList() << item.fileName(), (int)itemType::Dir),
                * diritem_proc = new QTreeWidgetItem(QStringList() << item.fileName(), (int)itemType::Dir);
            qint64 subItemCount = QDir(item.filePath(), "*", DefaultSort, DefaultFilter).count(), localPrimSize = 0, localProcSize = 0;

            GenerateDirTree(item.filePath(), subItemCount, localPrimSize, localProcSize, diritem_prim, diritem_proc);

            if (!subItemCount) {
                primsize += localPrimSize, scount--;

                diritem_prim->setText(1, AutoShowSize(localPrimSize)), SetItemBold(diritem_prim);
                insertTreeItem(ui.BOX_PrimitiveDirectView, diritem_prim, primparent);

				DelFileList.push_back(item.filePath());
            }
            else {
                primsize += localPrimSize, procsize += localProcSize;

                diritem_prim->setText(1, AutoShowSize(localPrimSize));
                insertTreeItem(ui.BOX_PrimitiveDirectView, diritem_prim, primparent);

                diritem_proc->setText(1, AutoShowSize(localProcSize));
                insertTreeItem(ui.BOX_ProcessedDirectView, diritem_proc, procparent);
            }
        }
        else if (item.isFile()) {
            QTreeWidgetItem* fileitem_prim = new QTreeWidgetItem(QStringList() << item.fileName() << AutoShowSize(item.size()), (int)itemType::File),
                * fileitem_proc = new QTreeWidgetItem(QStringList() << item.fileName() << AutoShowSize(item.size()), (int)itemType::File);

            if (isRemovableFile(item.suffix())) {
                primsize += item.size(), scount--;
                SetItemBold(fileitem_prim);
                insertTreeItem(ui.BOX_PrimitiveDirectView, fileitem_prim, primparent);

                DelFileList.push_back(item.filePath());
            }
            else {
                insertTreeItem(ui.BOX_PrimitiveDirectView, fileitem_prim, primparent);
                insertTreeItem(ui.BOX_ProcessedDirectView, fileitem_proc, procparent);
                primsize += item.size(), procsize += item.size();
            }

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

    case SizeUnits::Tera:
        return byte / 1099511627776.0;
		break;

    default:
        return byte;
    }
}

QString ProjectSimplifier::AutoShowSize(qint64 byte) {
    if (byte < (1 << 10))
        return QString("%1 B").arg(ShowSize(byte, SizeUnits::Byte));

    else if (byte < (1 << 20))
        return QString("%1 KB").arg(QString::number(ShowSize(byte, SizeUnits::Kilo), 'f', 2));

    else if (byte < (1 << 30))
        return QString("%1 MB").arg(QString::number(ShowSize(byte, SizeUnits::Mega), 'f', 2));

    else if (byte < ((qint64)1 << 40))
        return QString("%1 GB").arg(QString::number(ShowSize(byte, SizeUnits::Giga), 'f', 2));

    else
        return QString("%1 TB").arg(QString::number(ShowSize(byte, SizeUnits::Tera), 'f', 2));
}

void ProjectSimplifier::SetItemBold(QTreeWidgetItem* item) {
    QFont bold = item->font(0);
    bold.setBold(true);
    for (int i = 0; i <= item->columnCount() - 1; i++) item->setFont(i, bold);
}

void ProjectSimplifier::insertTreeItem(QTreeWidget* ptree, QTreeWidgetItem* item, QTreeWidgetItem* parent) {
    if (parent == nullptr)
        ptree->addTopLevelItem(item);
    else parent->addChild(item);
}