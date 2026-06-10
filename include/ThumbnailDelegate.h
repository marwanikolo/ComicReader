#pragma once
#include <QStyledItemDelegate>
#include <QFileSystemModel>
#include <QImageReader>
#include <QDirIterator>
#include <QPixmap>
#include <QHash>
#include <QFileInfo>
#include <QPainter>
#include <QApplication> // Needed for styling
#include <QDebug>

class ThumbnailDelegate : public QStyledItemDelegate {
    Q_OBJECT
private:
    // Changed from QIcon to QPixmap for direct, low-level painting
    mutable QHash<QString, QPixmap> pixmapCache; 
    QFileSystemModel* model;

    QString getFirstImage(const QString& dirPath) const {
        QStringList filters = {"*.jpg", "*.jpeg", "*.png", "*.webp"};
        QDirIterator it(dirPath, filters, QDir::Files, QDirIterator::Subdirectories);
        if (it.hasNext()) return it.next();
        return QString();
    }

public:
    ThumbnailDelegate(QFileSystemModel* model, QObject* parent = nullptr)
        : QStyledItemDelegate(parent), model(model) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        QString path = model->fileInfo(index).absoluteFilePath();
        QFileInfo info(path);

        QPixmap pixmap;

        // --- 1. Fetch or Generate the Pixmap ---
        if (pixmapCache.contains(path)) {
            pixmap = pixmapCache.value(path);
        } else {
            QString targetFile;
            
            if (info.isDir()) {
                targetFile = getFirstImage(path);
            } else if (info.isFile()) {
                QString lower = info.fileName().toLower();
                if (lower.endsWith(".jpg") || lower.endsWith(".png") || lower.endsWith(".webp")) {
                    targetFile = path;
                }
            }

            if (!targetFile.isEmpty()) {
                QImageReader reader(targetFile);
                reader.setAutoTransform(true);
                QSize origSize = reader.size();
                if (origSize.isValid()) {
                    reader.setScaledSize(origSize.scaled(150, 225, Qt::KeepAspectRatio));
                }
                QImage img = reader.read();
                if (!img.isNull()) {
                    pixmap = QPixmap::fromImage(img);
                    pixmapCache.insert(path, pixmap);
                }
            }
        }

        // --- 2. MANUAL PAINTING ENGINE ---
        
        // Draw the background (handles the blue highlight when you select/hover an item)
        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

        // Draw the Image (Centered in the upper portion of the grid cell)
        if (!pixmap.isNull()) {
            // Reserve the bottom 30 pixels for the text label
            QRect imageBox = opt.rect.adjusted(5, 5, -5, -30); 
            QPixmap scaled = pixmap.scaled(imageBox.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            
            // Calculate exact center coordinates
            int x = imageBox.left() + (imageBox.width() - scaled.width()) / 2;
            int y = imageBox.top() + (imageBox.height() - scaled.height()) / 2;
            
            painter->drawPixmap(x, y, scaled);
        }

        // Draw the Text (Centered at the bottom of the grid cell)
        if (!opt.text.isEmpty()) {
            QRect textRect = opt.rect.adjusted(5, opt.rect.height() - 30, -5, -5);
            
            // Ensure text color inverts properly when the item is highlighted
            if (opt.state & QStyle::State_Selected) {
                painter->setPen(opt.palette.color(QPalette::HighlightedText));
            } else {
                painter->setPen(opt.palette.color(QPalette::Text));
            }
            
            painter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, opt.text);
        }
    }

    // Force the grid to give us the exact amount of space we defined in ReaderWindow
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        return QSize(170, 260); 
    }
};
