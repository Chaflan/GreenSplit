#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "datacore.h"
#include "peoplemodel.h"
#include "transactionsmodel.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

// TODO: Dynamically resize the columns

// TODO: Put these at the model level or controller perhaps
void ReadFromJsonFile(PeopleModel* mPeopleModel, TransactionsModel* mTransactionsModel)
{
    QFile file("save.json");
    if (file.open(QIODevice::ReadOnly))
    {
        QJsonDocument jsonDoc(QJsonDocument::fromJson(file.readAll()));
        QJsonObject jsonObj = jsonDoc.object();
        mPeopleModel->jsonRead(jsonObj);
        mTransactionsModel->jsonRead(jsonObj);
    }
}

void WriteJson(PeopleModel* mPeopleModel, TransactionsModel* mTransactionsModel)
{
    QFile file("save.json");
    if (file.open(QIODevice::WriteOnly))
    {
        QJsonObject json;
        mPeopleModel->jsonWrite(json);
        mTransactionsModel->jsonWrite(json);
        QJsonDocument saveDoc(json);
        file.write(saveDoc.toJson());
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    DataCore dataCore;
    PeopleModel peopleModel(dataCore);
    TransactionsModel transactionsModel(dataCore);
    ReadFromJsonFile(&peopleModel, &transactionsModel);

    peopleModel.addPerson("C", "Chad");
    peopleModel.addPerson("J", "Jeremy");
    peopleModel.addPerson("Lq", "Lucas");
    peopleModel.addPerson("L", "QQQQQQQQQQQQQQQ");

    QQmlContext* context = engine.rootContext();
    context->setContextProperty("peopleModel", &peopleModel);
    context->setContextProperty("transactionsModel", &transactionsModel);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    auto ret = app.exec();
    WriteJson(&peopleModel, &transactionsModel);
    return ret;
}
