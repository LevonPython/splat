#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QQmlContext>
#include "QtSMSplat.h"
#include <QThread>
#include <QDir>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QtSMSplat::registerQtSMSplatQmlTypes();
    engine.addImportPath("qrc:/qml");
    QtSMSplat::init(&engine);
    const QUrl url(QStringLiteral("qrc:/qml/SMSplat/TestUI.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);
    return app.exec();
}