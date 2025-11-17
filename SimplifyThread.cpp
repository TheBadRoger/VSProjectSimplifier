#include "SimplifyThread.h"
#include "FunctionLibrary.h"

SimplifyThread::SimplifyThread() {}
SimplifyThread::~SimplifyThread() {}

void SimplifyThread::ReceiveRootDir(QString rootDir)
{
	RootDirStr = rootDir;
}

void SimplifyThread::threadShowDirInfo() {
	if (RootDirStr.isEmpty()) {
		emit ReturnErrorMsg("未选择目录");
        return;
    }

    qint64 PlaceHolder = 0;

    emit SetEnable_Main_BTN_OpenDir(false);
    emit SetEnable_Main_BTN_executeProcess(false);
    emit SetText_Main_LAB_PrimitiveSize("计算中...");
    emit SetText_Main_LAB_ProcessedSize("计算中...");
    emit SetText_Main_LAB_ShowRemovableSize("计算中...");
    emit SetText_Main_LAB_ShowPrimitiveCount("计算中...");
    emit SetText_Main_LAB_ShowProcessedCount("计算中...");

    GenerateDirTree(RootDirStr, PlaceHolder, PrimitiveSize, ProcessedSize);

    emit SetText_Main_statusBar("完成");
    emit SetText_Main_LAB_PrimitiveSize(FunctionLibrary::AutoShowSize(PrimitiveSize));
    emit SetText_Main_LAB_ProcessedSize(FunctionLibrary::AutoShowSize(ProcessedSize));
	emit SetText_Main_LAB_ShowPrimitiveCount(QString("%1 个文件，%2 个文件夹").arg(PrimitiveCount_File).arg(PrimitiveCount_Dir));
	emit SetText_Main_LAB_ShowProcessedCount(QString("%1 个文件，%2 个文件夹").arg(ProcessedCount_File).arg(ProcessedCount_Dir));

    emit SetEnable_Main_BTN_executeProcess(true);
    emit SetEnable_Main_BTN_OpenDir(true);
    emit SetText_Main_LAB_ShowRemovableSize(QString("- %1\n→").arg(FunctionLibrary::AutoShowSize(PrimitiveSize - ProcessedSize)));
}

void SimplifyThread::insertTreeItem(QTreeWidget* ptree, QTreeWidgetItem* item, QTreeWidgetItem* parent) {
    if (parent == nullptr)
        ptree->addTopLevelItem(item);
    else parent->addChild(item);
}

void SimplifyThread::GenerateDirTree(const QDir& local, qint64& scount, qint64& primsize, qint64& procsize, QTreeWidgetItem* primparent, QTreeWidgetItem* procparent) {
    for (const auto& item : local.entryInfoList(DefaultFilter, DefaultSort)) {
        emit SetText_Main_statusBar(QString("正在扫描：%1").arg(item.path()));

        if (item.isDir()) {
            QTreeWidgetItem* diritem_prim = new QTreeWidgetItem(QStringList() << item.fileName(), (int)itemType::Dir),
                * diritem_proc = new QTreeWidgetItem(QStringList() << item.fileName(), (int)itemType::Dir);
            qint64 subItemCount = QDir(item.filePath(), "*", DefaultSort, DefaultFilter).count(), localPrimSize = 0, localProcSize = 0;

            GenerateDirTree(item.filePath(), subItemCount, localPrimSize, localProcSize, diritem_prim, diritem_proc);

            if (!subItemCount) {
                PrimitiveCount_Dir++, primsize += localPrimSize, scount--;

                diritem_prim->setText(1, FunctionLibrary::AutoShowSize(localPrimSize)), FunctionLibrary::SetItemBold(diritem_prim);
                emit insertItem_BOX_PrimitiveView(diritem_prim, primparent);

                DelFileList.push_back(item.filePath());
            }
            else {
                PrimitiveCount_Dir++, ProcessedCount_Dir++, primsize += localPrimSize, procsize += localProcSize;

                diritem_prim->setText(1, FunctionLibrary::AutoShowSize(localPrimSize));
                emit insertItem_BOX_PrimitiveView(diritem_prim, primparent);

                diritem_proc->setText(1, FunctionLibrary::AutoShowSize(localProcSize));
                emit insertItem_BOX_ProcessedView(diritem_proc, procparent);
            }
        }
        else if (item.isFile()) {
            QTreeWidgetItem* fileitem_prim = new QTreeWidgetItem(QStringList() << item.fileName() << FunctionLibrary::AutoShowSize(item.size()), (int)itemType::File),
                * fileitem_proc = new QTreeWidgetItem(QStringList() << item.fileName() << FunctionLibrary::AutoShowSize(item.size()), (int)itemType::File);

            if (FunctionLibrary::isRemovableFile(item.suffix())) {
                FunctionLibrary::SetItemBold(fileitem_prim);
                emit insertItem_BOX_PrimitiveView(fileitem_prim, primparent);
                PrimitiveCount_File++, primsize += item.size(), scount--;

                DelFileList.push_back(item.filePath());
            }
            else {
                emit insertItem_BOX_PrimitiveView(fileitem_prim, primparent);
                emit insertItem_BOX_ProcessedView(fileitem_proc, procparent);
                PrimitiveCount_File++, ProcessedCount_File++, primsize += item.size(), procsize += item.size();
            }
        }
    }
}

void SimplifyThread::threadSimplifyDir() {
    if (DelFileList.isEmpty())
        SetText_Main_statusBar("没有可删除的文件");
    else {
        SetEnable_Main_BTN_executeProcess(false);
        for (const auto& item : DelFileList) {
            if (QFileInfo(item).isDir())
                QDir(item).removeRecursively();

            else if (QFileInfo(item).isFile())
                QFile(item).remove();

            SetText_Main_statusBar(QString("正在删除：%1").arg(item));
			DelFileList.erase(DelFileList.begin());
        }
        emit SetText_Main_statusBar("完成");
        emit SetEnable_Main_BTN_OpenDir(true);
		emit SetEnable_Main_BTN_executeProcess(true);
    }
}