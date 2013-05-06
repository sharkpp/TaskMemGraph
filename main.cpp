#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "processlist.h"
#include <Security/Authorization.h>

int acquireTaskportRight() {
    AuthorizationRef authorization;
    AuthorizationFlags flags
            = kAuthorizationFlagExtendRights
            | kAuthorizationFlagPreAuthorize
            | kAuthorizationFlagInteractionAllowed
            | ( 1 << 5);
    OSStatus status = AuthorizationCreate (NULL, kAuthorizationEmptyEnvironment, flags/*kAuthorizationFlagDefaults*/, &authorization);
    if (status != 0) {
        fprintf(stderr, "Error creating authorization reference\\n");
        return -1;
    }
    AuthorizationItem right = { "system.privilege.taskport", 0, 0 , 0 };
    AuthorizationItem items[] = { right };
    AuthorizationRights rights = { sizeof(items) / sizeof(items[0]), items };
    //AuthorizationFlags flags = kAuthorizationFlagInteractionAllowed | kAuthorizationFlagExtendRights | kAuthorizationFlagPreAuthorize;

    status = AuthorizationCopyRights (authorization, &rights, kAuthorizationEmptyEnvironment, flags, NULL);
    if (status != 0) {
        fprintf(stderr, "Error authorizing current process with right to call task_for_pid\\n");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    fprintf(stderr, "****\n");
    if( acquireTaskportRight() < 0 )
    {
        return -1;
    }

    QGuiApplication app(argc, argv);

    // 型の登録
    qmlRegisterType<ProcessInfo>("ProcessLib",1,0,"ProcessInfo");
    qmlRegisterType<ProcessList>("ProcessLib",1,0,"ProcessList");

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
