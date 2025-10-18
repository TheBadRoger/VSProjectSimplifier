#include "ProjectSimplifier.h"
#include <QFileDialog>
#include <QMessageBox>

ProjectSimplifier::ProjectSimplifier(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    SelectedDirPath = QCoreApplication::applicationDirPath();

	ui.LAB_ShowRemovableSize->setText("");
	ui.LAB_ShowPrimitiveCount->setText("");
	ui.LAB_ShowProcessedCount->setText("");

    Simplifier->moveToThread(ProcessingThread);

	connect(this, SIGNAL(SendRootDir(QString)), Simplifier, SLOT(ReceiveRootDir(QString)));
	connect(this, SIGNAL(executeShowDirInfo()), Simplifier, SLOT(threadShowDirInfo()));
	connect(this, SIGNAL(executeSimplifyDir()), Simplifier, SLOT(threadSimplifyDir()));
	connect(Simplifier, SIGNAL(ReturnErrorMsg(QString)), this, SLOT(ShowErrorMsg(QString)));
	connect(Simplifier, SIGNAL(SetMainUIEnable(QLabel*, bool)), this, SLOT(UITextUpdate(QLabel*, QString)));
	connect(Simplifier, SIGNAL(SetText_Main_statusBar(QString)), this, SLOT(UpdateText_statusBar(QString)));
	connect(Simplifier, SIGNAL(SetText_Main_LAB_PrimitiveSize(QString)), this, SLOT(UpdateText_LAB_PrimitiveSize(QString)));
	connect(Simplifier, SIGNAL(SetText_Main_LAB_ProcessedSize(QString)), this, SLOT(UpdateText_LAB_ProcessedSize(QString)));
	connect(Simplifier, SIGNAL(SetText_Main_LAB_ShowRemovableSize(QString)), this, SLOT(UpdateText_LAB_ShowRemovableSize(QString)));
	connect(Simplifier, SIGNAL(SetText_Main_LAB_ShowPrimitiveCount(QString)), this, SLOT(UpdateText_LAB_ShowPrimitiveCount(QString)));
	connect(Simplifier, SIGNAL(SetText_Main_LAB_ShowProcessedCount(QString)), this, SLOT(UpdateText_LAB_ShowProcessedCount(QString)));
	connect(Simplifier, SIGNAL(SetEnable_Main_BTN_OpenDir(bool)), this, SLOT(UpdateEnable_BTN_OpenDir(bool)));
	connect(Simplifier, SIGNAL(SetEnable_Main_BTN_executeProcess(bool)), this, SLOT(UpdateEnable_BTN_executeProcess(bool)));
	connect(Simplifier, SIGNAL(insertItem_BOX_PrimitiveView(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(insertItem_BOX_PrimitiveView(QTreeWidgetItem*, QTreeWidgetItem*)));
	connect(Simplifier, SIGNAL(insertItem_BOX_ProcessedView(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(insertItem_BOX_ProcessedView(QTreeWidgetItem*, QTreeWidgetItem*)));
}

ProjectSimplifier::~ProjectSimplifier() {}

void ProjectSimplifier::GetDirect() {
    QString tmpPath = QFileDialog::getExistingDirectory(this, "选择目录", SelectedDirPath);
    if (!tmpPath.isEmpty()) {
        SelectedDirPath = tmpPath;
        ui.BOX_InputFilePath->setText(SelectedDirPath);
		ShowDirInfo();
    }
}

void ProjectSimplifier::SimplifyDirect() {
	emit executeSimplifyDir();
}

void ProjectSimplifier::ShowErrorMsg(QString msg) {
	QMessageBox::critical(this, "错误", msg, QMessageBox::Ok);
}

void ProjectSimplifier::UpdateText_statusBar(QString text) {
	ui.statusBar->showMessage(text);
}

void ProjectSimplifier::UpdateText_LAB_PrimitiveSize(QString text) {
	ui.LAB_PrimitiveSize->setText(text);
}

void ProjectSimplifier::UpdateText_LAB_ProcessedSize(QString text) {
	ui.LAB_ProcessedSize->setText(text);
}

void ProjectSimplifier::UpdateText_LAB_ShowRemovableSize(QString text) {
	ui.LAB_ShowRemovableSize->setText(text);
}

void ProjectSimplifier::UpdateText_LAB_ShowPrimitiveCount(QString text) {
	ui.LAB_ShowPrimitiveCount->setText(text);
}

void ProjectSimplifier::UpdateText_LAB_ShowProcessedCount(QString text) {
	ui.LAB_ShowProcessedCount->setText(text);
}

void ProjectSimplifier::UpdateEnable_BTN_OpenDir(bool enable) {
	ui.BTN_OpenDir->setEnabled(enable);
}

void ProjectSimplifier::UpdateEnable_BTN_executeProcess(bool enable) {
	ui.BTN_executeProcess->setEnabled(enable);
}

void ProjectSimplifier::insertItem_BOX_PrimitiveView(QTreeWidgetItem* item, QTreeWidgetItem* parent) {
	if (parent == nullptr)
		ui.BOX_PrimitiveDirectView->addTopLevelItem(item);
	else parent->addChild(item);
}

void ProjectSimplifier::insertItem_BOX_ProcessedView(QTreeWidgetItem* item, QTreeWidgetItem* parent) {
	if (parent == nullptr)
		ui.BOX_ProcessedDirectView->addTopLevelItem(item);
	else parent->addChild(item);
}

void ProjectSimplifier::ClearUI() {
    ui.BOX_PrimitiveDirectView->clear();
    ui.BOX_ProcessedDirectView->clear();
}

void ProjectSimplifier::ShowDirInfo() {
    ClearUI();
	emit SendRootDir(SelectedDirPath);
	ProcessingThread->start();
    emit executeShowDirInfo();
	ui.BOX_PrimitiveDirectView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui.BOX_ProcessedDirectView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}
