#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("vehlwn-MotionDetection");

    MainWindow w;
    w.show();
    return a.exec();
}
