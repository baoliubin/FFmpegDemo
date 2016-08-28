#include "mainwindow.h"
#include <QApplication>
#include <QQmlEngine>
#include <QQuickView>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	MainWindow w;
	w.show();
//	QQuickView view;
//	view.setSource(QUrl(QString("qrc:/qml/main.qml")));
//	QObject::connect(view.engine(), SIGNAL(quit()), qApp, SLOT(quit()));
//	view.show();
    return a.exec();
}
