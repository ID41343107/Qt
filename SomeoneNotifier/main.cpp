#include "someonenotifier.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SomeoneNotifier w;
    w.show();
    return a.exec();
}
