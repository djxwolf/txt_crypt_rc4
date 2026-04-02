#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set Fusion style for modern cross-platform appearance
    app.setStyle("Fusion");

    MainWindow window;
    window.show();

    return app.exec();
}
