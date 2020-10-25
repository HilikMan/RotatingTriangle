#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>

#include "triangle.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Triangle>("OpenGLUnderQML", 1, 0, "Triangle");

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
