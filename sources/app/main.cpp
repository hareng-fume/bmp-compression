#include "filelistmodel.h"
#include "proxymodel.h"

#include <core.h>

#ifdef DEBUG_BUILD
#include "test.h"
#endif

#include <QCommandLineParser>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption(
        {{"d", "dir"},
         QCoreApplication::translate("main", "Read *.bmp, *.png, *.barch files from <directory>."),
         QCoreApplication::translate("main", "directory"),
         QDir::currentPath()});
#ifdef DEBUG_BUILD
    parser.addOption(
        {{"t", "test"}, QCoreApplication::translate("main", "Launch unit-tests, Debug only.")});
#endif
    parser.process(app);

#ifdef DEBUG_BUILD
    if (parser.isSet("test")) {
        _Test::_run_tests();
    }
#endif

    const auto dirVal = parser.value("dir");
    const auto directory = dirVal.isEmpty() ? QDir::currentPath() : QDir(dirVal).absolutePath();
    auto p_model = FileListModel::Create(directory);
    ProxyModel proxyModel;
    proxyModel.setSourceModel(p_model);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("proxyModel", &proxyModel);
    engine.rootContext()->setContextProperty("workingDirectory", directory);
    engine.rootContext()->setContextProperty("mainAppWindow", &app);

    const QUrl url(QStringLiteral("qrc:/app/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
