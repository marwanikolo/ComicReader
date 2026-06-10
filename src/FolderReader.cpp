#include "FolderReader.h"
#include <QDirIterator>
#include <QCollator>
#include <algorithm>

bool FolderReader::openFolder(const QString& folderPath) {
    pageList.clear();
    QStringList nameFilters = {"*.jpg", "*.jpeg", "*.png", "*.webp"};
    
    // Removed Subdirectories flag. We only want files in this specific folder now.
    QDirIterator it(folderPath, nameFilters, QDir::Files | QDir::NoDotAndDotDot);
    
    while (it.hasNext()) {
        pageList.append(it.next());
    }

    QCollator collator;
    collator.setNumericMode(true);
    std::sort(pageList.begin(), pageList.end(), 
              [&collator](const QString& a, const QString& b) {
                  return collator.compare(a, b) < 0;
              });

    return !pageList.isEmpty();
}

int FolderReader::getPageCount() const { return pageList.size(); }

QString FolderReader::getPagePath(int index) const {
    if (index < 0 || index >= pageList.size()) return {};
    return pageList[index];
}

// Helper to start reading exactly where the user clicked
int FolderReader::findPageIndex(const QString& absolutePath) const {
    return pageList.indexOf(absolutePath);
}
