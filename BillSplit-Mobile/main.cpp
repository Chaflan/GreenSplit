#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "datacoreobject.h"
#include "peopletablemodel.h"
#include "transactionstablemodel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // Type registration
    qmlRegisterType<DataCoreObject>("com.company.core", 1,0, "DataCore");
    qmlRegisterType<PeopleTableModel>("com.company.core", 1,0, "PeopleTableModel");
    qmlRegisterType<TransactionsTableModel>("com.company.core", 1,0, "TransactionsTableModel");
    qmlRegisterType<TransactionModel>("com.company.core", 1,0, "TransactionModel");

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    auto ret = app.exec();
    return ret;
}
