#include "rjmanager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RJManager w;
    w.show();

    return a.exec();
}
