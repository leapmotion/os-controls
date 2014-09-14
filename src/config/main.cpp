#include "configwindow.h"
#include <QApplication>
#include "utility/PlatformInitializer.h"

int main(int argc, char *argv[])
{
    PlatformInitializer workingDirectoryChanger;

    QApplication a(argc, argv);
    ConfigWindow w;
    w.show();

    return a.exec();
}
