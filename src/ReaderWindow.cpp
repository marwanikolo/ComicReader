#include "ReaderWindow.h"
#include "ThumbnailDelegate.h"
#include <QKeyEvent>
#include <QApplication>
#include <QScrollBar>
#include <algorithm> // for std::clamp

// Initialize pointers to nullptr to prevent dereferencing garbage memory during Qt's early init
ReaderWindow::ReaderWindow(QWidget* parent) : QMainWindow(parent), stackedWidget(nullptr), browserView(nullptr), fileModel(nullptr), imageScrollArea(nullptr), imageLabel(nullptr), currentPage(0), zoomFactor(1.5) {
    
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // --- 1. Browser View Setup ---
    browserView = new QListView(this);
    browserView->setStyleSheet("background-color: #1e1e1e; color: #ffffff; font-size: 14px; outline: none;");
    
    browserView->setViewMode(QListView::IconMode);
    
    // CRITICAL: These must be set so Qt allocates physical space to paint the icons
    browserView->setIconSize(QSize(150, 225));
    browserView->setGridSize(QSize(170, 260));
    
    browserView->setResizeMode(QListView::Adjust);
    browserView->setSpacing(10);
    browserView->setUniformItemSizes(true);
    
    // Use standard safe file model
    fileModel = new QFileSystemModel(this);
    fileModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    fileModel->setNameFilters({"*.jpg", "*.jpeg", "*.png", "*.webp"});
    fileModel->setNameFilterDisables(false);
    browserView->setModel(fileModel);
    
    // Attach the Delegate to handle lazy-loading thumbnails safely
    browserView->setItemDelegate(new ThumbnailDelegate(fileModel, this));
    
    connect(browserView, &QListView::doubleClicked, this, &ReaderWindow::onBrowserDoubleClicked);

    // --- 2. Reader View Setup (with Zoom/Scroll) ---
    imageLabel = new QLabel(); 
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet("background-color: #121212;");
    
    imageScrollArea = new QScrollArea(this);
    imageScrollArea->setWidget(imageLabel);
    imageScrollArea->setAlignment(Qt::AlignCenter);
    imageScrollArea->setStyleSheet("background-color: #121212; border: none;");

    stackedWidget->addWidget(browserView);     
    stackedWidget->addWidget(imageScrollArea); 

    // Install filters ONLY after all widgets have been safely created
    browserView->installEventFilter(this);
    imageScrollArea->viewport()->installEventFilter(this);

    resize(1000, 800);
}

void ReaderWindow::setBrowserRoot(const QString& path) {
    if (!fileModel || !browserView) return;
    fileModel->setRootPath(path);
    browserView->setRootIndex(fileModel->index(path));
    setWindowTitle("ArchReader - Browser");
}

void ReaderWindow::openSelectedItem() {
    if (!browserView || !fileModel) return;
    QModelIndex index = browserView->currentIndex();
    if (!index.isValid()) return;

    if (fileModel->isDir(index)) {
        browserView->setRootIndex(index); // Enter directory
    } else {
        // Open the reader
        QString folderPath = fileModel->fileInfo(index).absolutePath();
        QString clickedFile = fileModel->fileInfo(index).absoluteFilePath();

        if (reader.openFolder(folderPath)) {
            currentPage = reader.findPageIndex(clickedFile);
            if (currentPage < 0) currentPage = 0;

            if (stackedWidget) stackedWidget->setCurrentIndex(1);
            renderPage();
        }
    }
}

void ReaderWindow::onBrowserDoubleClicked(const QModelIndex& index) {
    openSelectedItem();
}

void ReaderWindow::keyPressEvent(QKeyEvent* event) {
    if (stackedWidget && stackedWidget->currentIndex() == 1) { 
        // --- READER CONTROLS (Yazi style) ---
        if (event->key() == Qt::Key_L || event->key() == Qt::Key_Space || event->key() == Qt::Key_Right) {
            if (currentPage < reader.getPageCount() - 1) {
                currentPage++;
                renderPage();
            }
        } else if (event->key() == Qt::Key_H || event->key() == Qt::Key_Left) {
            if (currentPage > 0) {
                currentPage--;
                renderPage();
            }
        } else if (event->key() == Qt::Key_Q || event->key() == Qt::Key_Escape) {
            stackedWidget->setCurrentIndex(0);
            currentPixmap = QPixmap(); // Free memory
            if (imageLabel) imageLabel->clear();
            setWindowTitle("ArchReader - Browser");
        }
    }
    QMainWindow::keyPressEvent(event);
}

bool ReaderWindow::eventFilter(QObject *obj, QEvent *event) {
    // --- BROWSER NAVIGATION ---
    if (browserView && obj == browserView && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        
        if (keyEvent->key() == Qt::Key_H || keyEvent->key() == Qt::Key_Backspace) {
            QModelIndex currentIndex = browserView->rootIndex();
            QModelIndex parentIndex = fileModel->parent(currentIndex);
            if (parentIndex.isValid()) {
                browserView->setRootIndex(parentIndex);
            }
            return true; 
        } 
        else if (keyEvent->key() == Qt::Key_L || keyEvent->key() == Qt::Key_Return) {
            openSelectedItem();
            return true;
        } 
        else if (keyEvent->key() == Qt::Key_J) {
            QKeyEvent downEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
            QCoreApplication::sendEvent(browserView, &downEvent);
            return true;
        } 
        else if (keyEvent->key() == Qt::Key_K) {
            QKeyEvent upEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
            QCoreApplication::sendEvent(browserView, &upEvent);
            return true;
        } 
        else if (keyEvent->key() == Qt::Key_Q) {
            close();
            return true;
        }
        return false; 
    }

    // --- READER ZOOM & SCROLL ---
    if (imageScrollArea && imageScrollArea->viewport() && obj == imageScrollArea->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        
        if (wheelEvent->modifiers() & Qt::ControlModifier) {
            if (wheelEvent->angleDelta().y() > 0) zoomFactor *= 1.15; 
            else zoomFactor /= 1.15; 
            
            zoomFactor = std::clamp(zoomFactor, 0.1, 5.0);
            scaleAndDisplayImage();
            return true; 
        } 
        else {
            if (zoomFactor <= 1.01) {
                if (wheelEvent->angleDelta().y() > 0) { 
                    if (currentPage > 0) {
                        currentPage--;
                        renderPage();
                    }
                } else if (wheelEvent->angleDelta().y() < 0) { 
                    if (currentPage < reader.getPageCount() - 1) {
                        currentPage++;
                        renderPage();
                    }
                }
                return true; 
            }
            return false; 
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void ReaderWindow::resizeEvent(QResizeEvent* event) {
    if (stackedWidget && stackedWidget->currentIndex() == 1 && !currentPixmap.isNull()) {
        scaleAndDisplayImage();
    }
    QMainWindow::resizeEvent(event);
}

void ReaderWindow::renderPage() {
    QString imagePath = reader.getPagePath(currentPage);
    if (imagePath.isEmpty()) return;

    currentPixmap.load(imagePath);
    zoomFactor = 1.5; 
    scaleAndDisplayImage();
    
    setWindowTitle(QString("ArchReader - Page %1 / %2")
                   .arg(currentPage + 1)
                   .arg(reader.getPageCount()));
}

void ReaderWindow::scaleAndDisplayImage() {
    if (currentPixmap.isNull() || !imageScrollArea || !imageLabel) return;

    QSize baseSize = imageScrollArea->size() - QSize(4, 4);
    QSize targetSize = baseSize * zoomFactor;

    QPixmap scaled = currentPixmap.scaled(
        targetSize,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );
    
    imageLabel->setPixmap(scaled);
    imageLabel->resize(scaled.size());
}
