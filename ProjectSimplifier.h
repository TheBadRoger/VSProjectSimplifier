#pragma once

#include <QtWidgets/QMainWindow>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QSet>
#include <QThread>
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
    enum class SizeUnits { Byte, Kilo, Mega, Giga, Tera };
    enum class itemType { Dir, File };
    QString RootPath;
    QDir::Filters DefaultFilter = (QDir::Files | QDir::AllDirs | QDir::NoSymLinks | QDir::Hidden | QDir::CaseSensitive | QDir::NoDotAndDotDot);
    QDir::SortFlags DefaultSort = (QDir::DirsFirst | QDir::Name);
	QVector<QString> DelFileList;

    qint64 PrimitiveSize = 0;
    qint64 ProcessedSize = 0;
    
    const QSet<QString> extList = { "db","tlog","log","idb","pdb","lastbuildstate",
                                                  "ilk","exp","obj","iobj","ipch","tlh","enc","tli","exp" };
    void ShowDirectOnBox();
    void ClearData();
    void GenerateDirTree(const QDir& local, qint64& scount, qint64& primsize, qint64& procsize, QTreeWidgetItem* primparent = nullptr, QTreeWidgetItem* procparent = nullptr);
    void DeleteFile(const QString& localpath);
    bool CheckValidDirect();

    void insertTreeItem(QTreeWidget* ptree, QTreeWidgetItem* item, QTreeWidgetItem* parent = nullptr);

    bool isRemovableFile(const QString& fext);
    double ShowSize(qint64 byte, SizeUnits unit);
    QString AutoShowSize(qint64 byte);
    void SetItemBold(QTreeWidgetItem* item);
};