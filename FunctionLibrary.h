#pragma once

#include <QObject>
#include <QDir>

QDir::Filters DefaultFilter = (QDir::Files | QDir::AllDirs | QDir::NoSymLinks | QDir::Hidden | QDir::CaseSensitive | QDir::NoDotAndDotDot);
QDir::SortFlags DefaultSort = (QDir::DirsFirst | QDir::Name);
enum class SizeUnits { Byte, Kilo, Mega, Giga, Tera };
enum class itemType { Dir, File };

const QSet<QString> extList = { "db","tlog","log","idb","pdb","lastbuildstate",
                                                  "ilk","exp","obj","iobj","ipch","tlh","enc","tli","exp" };

class FunctionLibrary
{
public:
    FunctionLibrary() {}
    ~FunctionLibrary() {}

	static double ShowSize(qint64 bytes, SizeUnits unit) {
        switch (unit) {
        case SizeUnits::Byte:
            return bytes;
            break;

        case SizeUnits::Kilo:
            return bytes / 1024.0;
            break;

        case SizeUnits::Mega:
            return bytes / 1048576.0;
            break;

        case SizeUnits::Giga:
            return bytes / 1073741824.0;
            break;

        case SizeUnits::Tera:
            return bytes / 1099511627776.0;
            break;

        default:
            return bytes;
        }
	}

    static QString AutoShowSize(qint64 bytes) {
        if (bytes < (1 << 10))
            return QString("%1 B").arg(ShowSize(bytes, SizeUnits::Byte));

        else if (bytes < (1 << 20))
            return QString("%1 KB").arg(QString::number(ShowSize(bytes, SizeUnits::Kilo), 'f', 2));

        else if (bytes < (1 << 30))
            return QString("%1 MB").arg(QString::number(ShowSize(bytes, SizeUnits::Mega), 'f', 2));

        else if (bytes < ((qint64)1 << 40))
            return QString("%1 GB").arg(QString::number(ShowSize(bytes, SizeUnits::Giga), 'f', 2));

        else
            return QString("%1 TB").arg(QString::number(ShowSize(bytes, SizeUnits::Tera), 'f', 2));
    }

    static void SetItemBold(QTreeWidgetItem* item) {
        QFont bold = item->font(0);
        bold.setBold(true);
        for (int i = 0; i <= item->columnCount() - 1; i++) item->setFont(i, bold);
    }

    static bool isRemovableFile(const QString& fext) {
        if (extList.find(fext) == extList.end()) return false;
        else return true;
    }
};

