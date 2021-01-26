#include "datacoreobject.h"
#include "peopletablemodel.h"
#include "transactionstablemodel.h"
#include "resultsmodel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::quit,
        [&app](){
            qDebug() << "Engine called quit, telling app to quit too.";
            app.quit();
        });

    app.setOrganizationName("OrgName");
    app.setOrganizationDomain("OrgDomain");

    qmlRegisterType<DataCoreObject>("com.company.core", 1,0, "DataCore");
    qmlRegisterType<PeopleTableModel>("com.company.core", 1,0, "PeopleTableModel");
    qmlRegisterType<TransactionsTableModel>("com.company.core", 1,0, "TransactionsTableModel");
    qmlRegisterType<TransactionModel>("com.company.core", 1,0, "TransactionModel");
    qmlRegisterType<ResultsModel>("com.company.core", 1,0, "ResultsModel");

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    auto ret = app.exec();

    qDebug() << "Program exiting with " << ret;
    return ret;
}
