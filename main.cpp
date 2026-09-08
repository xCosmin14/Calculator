#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Calculator.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    app.setOrganizationName("Cosmin Nichita");
    app.setApplicationDisplayName("Calculator");
    app.setApplicationName("Calculator");

    Calculator calc;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("calc", &calc);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Calculator", "Main");

    return QGuiApplication::exec();
}
