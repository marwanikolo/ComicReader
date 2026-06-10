#pragma once
#include <QString>
#include <QStringList>

class FolderReader {
private:
    QStringList pageList;

public:
    bool openFolder(const QString& folderPath);
    int getPageCount() const;
    QString getPagePath(int index) const;
    int findPageIndex(const QString& absolutePath) const; // New: locate clicked file
};
