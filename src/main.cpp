#include <QApplication>
#include <QDebug>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qDebug() << "Application initialized.";

    MainWindow w;
    qDebug() << "MainWindow created.";

    w.show();
    qDebug() << "MainWindow shown.";

    return app.exec();
}
