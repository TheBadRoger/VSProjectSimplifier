#pragma once

#include <QtWidgets/QMainWindow>
#include <QStandardItemModel>
#include <QFileDialog>
#include <unordered_set>
#include "ui_ProjectSimplifier.h"

class ProjectSimplifier : public QMainWindow
{
    Q_OBJECT

public:
    ProjectSimplifier(QWidget *parent = nullptr);
    ~ProjectSimplifier();

    void initalizeUI();

public slots:
    void executeSimpifyDirect();
    void GetDirect();

private:
    Ui::ProjectSimplifierClass ui;
    enum class SizeUnits { Byte, Kilo, Mega, Giga };
    enum class itemType { Dir, File };
    QString RootPath;
    QDir::Filters DefaultFilter = (QDir::Files | QDir::AllDirs | QDir::NoSymLinks | QDir::Hidden | QDir::CaseSensitive | QDir::NoDotAndDotDot);
    QDir::SortFlags DefaultSort = (QDir::DirsFirst | QDir::Name);
    qint64 PrimitiveSize = 0;
    qint64 ProcessedSize = 0;
    qint64 CountCatch = LLONG_MAX;
    const std::unordered_set<QString> extList = { "db","tlog","log","idb","pdb","lastbuildstate",
                                                  "ilk","exp","obj","iobj","ipch","tlh","enc","tli","exp" };
    void ShowDirectOnBox();
    void ClearTree(QTreeWidgetItem* parent);
    void ClearData();
    bool CheckValidDirect();

    void GeneratePrimitiveTree(const QString& localpath, qint64& scount, qint64& fsize, QTreeWidgetItem* parent = nullptr);
    void GenerateProcessedTree(const QString& localpath, qint64& scount, qint64& fsize, QTreeWidgetItem* parent = nullptr);
    void insertTreeItem(QTreeWidget* ptree, QTreeWidgetItem* item, QTreeWidgetItem* parent = nullptr);
    void DeleteFile(const QString& localpath);

    bool isRemovableFile(const QString& fext);
    double ShowSize(qint64 byte, SizeUnits unit);
    QString AutoShowSize(qint64 byte);
    void SetItemBold(QTreeWidgetItem* item);
};