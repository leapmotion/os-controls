#include "configwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConfigWindow w;
    w.show();

    return a.exec();
}
