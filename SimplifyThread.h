#pragma once

#include <QObject>
#include <QDir>
#include "ui_ProjectSimplifier.h"

class SimplifyThread : public QObject
{
	Q_OBJECT

public:
	SimplifyThread();
	~SimplifyThread();

public slots:
	void ReceiveRootDir(QString rootDir);
	void threadShowDirInfo();
	void threadSimplifyDir();

signals:
	void ReturnErrorMsg(QString msg);
	void SetText_Main_statusBar(QString text);
	void SetText_Main_LAB_PrimitiveSize(QString text);
	void SetText_Main_LAB_ProcessedSize(QString text);
	void SetText_Main_LAB_ShowRemovableSize(QString text);
	void SetText_Main_LAB_ShowPrimitiveCount(QString text);
	void SetText_Main_LAB_ShowProcessedCount(QString text);
	void SetEnable_Main_BTN_OpenDir(bool enable);
	void SetEnable_Main_BTN_executeProcess(bool enable);
	void insertItem_BOX_PrimitiveView(QTreeWidgetItem* item, QTreeWidgetItem* parent = nullptr);
	void insertItem_BOX_ProcessedView(QTreeWidgetItem* item, QTreeWidgetItem* parent = nullptr);

private:
	QString RootDirStr;
	QVector<QString> DelFileList;
	qint64 PrimitiveSize = 0, ProcessedSize = 0, PrimitiveCount_File = 0, ProcessedCount_File = 0, PrimitiveCount_Dir = 0, ProcessedCount_Dir = 0;

	void insertTreeItem(QTreeWidget* tree, QTreeWidgetItem* item, QTreeWidgetItem* parent = nullptr);
	void GenerateDirTree(const QDir& local, qint64& scount, qint64& primsize, qint64& procsize, QTreeWidgetItem* primparent = nullptr, QTreeWidgetItem* procparent = nullptr);
};