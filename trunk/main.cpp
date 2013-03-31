#include <locale.h>
#include <QApplication>
#include <QMessageBox>
#include "openal.h"
#include "mainwindow.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    setlocale(LC_NUMERIC, "C");
    if (!loadOpenAL()) {
        qDebug() << "OpenAL32 loading error.";
        return 1;
    }
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    int exitCode = app.exec();
    unloadOpenAL();
    return exitCode;
}
