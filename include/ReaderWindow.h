#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QPixmap>
#include <QStackedWidget>
#include <QListView>
#include <QScrollArea>
#include <QWheelEvent>
#include <QFileSystemModel>
#include "FolderReader.h"

class ReaderWindow : public QMainWindow {
    Q_OBJECT

private:
    QStackedWidget* stackedWidget;
    
    QListView* browserView;
    QFileSystemModel* fileModel; // Back to the native, safe model

    FolderReader reader;
    QScrollArea* imageScrollArea;
    QLabel* imageLabel;
    int currentPage;
    QPixmap currentPixmap;
    double zoomFactor;

    void renderPage();
    void scaleAndDisplayImage();
    void openSelectedItem();

private slots:
    void onBrowserDoubleClicked(const QModelIndex& index);

public:
    explicit ReaderWindow(QWidget* parent = nullptr);
    void setBrowserRoot(const QString& path);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject *obj, QEvent *event) override; 
};
