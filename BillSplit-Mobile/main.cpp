#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "datacore.h"
#include "peoplemodel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    DataCore dataCore;
    PeopleModel peopleModel(dataCore);
    peopleModel.addPerson("C", "Chad");
    peopleModel.addPerson("J", "Jeremy");
    peopleModel.addPerson("L", "Lucas");
    peopleModel.addPerson("Q", "QQQQQQQQQQQQQQQ");

    QQmlContext* context = engine.rootContext();
    context->setContextProperty("peopleModel", &peopleModel);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
