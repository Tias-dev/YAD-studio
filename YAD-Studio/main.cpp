#include <qtresource.h>

#include <QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QFont f = a.font();
    Q_INIT_RESOURCE(Resources);
    MainWindow w;
    w.show();

    return a.exec();
}
