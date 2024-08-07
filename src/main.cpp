#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile styleFile(":/main.qss");
    if(styleFile.open(QIODevice::ReadOnly))
    {
        QString styleSheet = styleFile.readAll();
        styleFile.close();
        a.setStyleSheet(styleSheet);
    }

    MainWindow w;
    w.show();
    return a.exec();
}
