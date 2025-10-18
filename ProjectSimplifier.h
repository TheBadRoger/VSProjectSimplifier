#pragma once

#include <QtWidgets/QMainWindow>
#include <QThread>
#include "ui_ProjectSimplifier.h"
#include "SimplifyThread.h"

class ProjectSimplifier : public QMainWindow
{
    Q_OBJECT

public slots:
	void ShowErrorMsg(QString msg);
    void GetDirect();
    void SimplifyDirect();
    void UpdateText_statusBar(QString text);
	void UpdateText_LAB_PrimitiveSize(QString text);
	void UpdateText_LAB_ProcessedSize(QString text);
	void UpdateText_LAB_ShowRemovableSize(QString text);
	void UpdateText_LAB_ShowPrimitiveCount(QString text);
	void UpdateText_LAB_ShowProcessedCount(QString text);
	void UpdateEnable_BTN_OpenDir(bool enable);
	void UpdateEnable_BTN_executeProcess(bool enable);
	void insertItem_BOX_PrimitiveView(QTreeWidgetItem* item, QTreeWidgetItem* parent = nullptr);
	void insertItem_BOX_ProcessedView(QTreeWidgetItem* item, QTreeWidgetItem* parent = nullptr);

signals:
	void SendRootDir(QString rootDir);
	void executeShowDirInfo();
	void executeSimplifyDir();

public:
    ProjectSimplifier(QWidget *parent = nullptr);
    ~ProjectSimplifier();

private:
	Ui::ProjectSimplifierClass ui;
    SimplifyThread* Simplifier = new SimplifyThread();
    QThread* ProcessingThread = new QThread();
    
	QString SelectedDirPath;

    void ClearUI();
    void ShowDirInfo();
};

