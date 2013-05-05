#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "processlist.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 型の登録
    qmlRegisterType<ProcessInfo>("ProcessLib",1,0,"ProcessInfo");
    qmlRegisterType<ProcessList>("ProcessLib",1,0,"ProcessList");

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
