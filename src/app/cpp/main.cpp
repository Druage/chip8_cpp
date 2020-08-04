#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "RenderSurface.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    qmlRegisterType<RenderSurface>("Chip8", 1, 0, "RenderSurface");

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/main.qml"));

    return app.exec();
}
