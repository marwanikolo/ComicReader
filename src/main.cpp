#include <QApplication>
#include <QStringList>
#include "ReaderWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ReaderWindow window;
    
    QStringList args = app.arguments();
    if (args.size() > 1) {
        window.setBrowserRoot(args.at(1));
    } else {
        // Fallback to the current user's home directory if no path is provided
        window.setBrowserRoot(QDir::homePath());
    }

    window.show();
    return app.exec();
}
