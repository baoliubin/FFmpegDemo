#include <QApplication>
#include "window.h"
int main(int argc, char * argv[])
{
    qSetMessagePattern("log[%{file} %{line} %{function} %{threadid}] %{message}");
    QApplication a(argc, argv);
    Window w;
    w.show();
    return a.exec();
}
