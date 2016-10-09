#include <QApplication>
#include <QScreen>

#define US_OPENGL

#ifdef US_OPENGL
#include "openglwindow.h"
#else
#include "widget.h"
#endif
int main(int argc, char * argv[])
{
    qSetMessagePattern("log[%{file} %{line} %{function} %{threadid}] %{message}");
    QApplication a(argc, argv);
#ifdef US_OPENGL
    OpenGLWindow w;
//    w.resize(a.primaryScreen()->size());
    w.show();
#else
    Widget widget;
    widget.show();
#endif
    return a.exec();
}
