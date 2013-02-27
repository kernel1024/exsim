#include <locale.h>
#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    setlocale(LC_NUMERIC, "C");
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
