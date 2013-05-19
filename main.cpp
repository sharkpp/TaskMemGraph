#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "processlist.h"

#if defined(Q_OS_MAC)
# include <Security/Authorization.h>
#elif defined(Q_OS_WIN32)
#else
#error "not impliment1"
#endif

#if defined(Q_OS_MAC)
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
#endif

int main(int argc, char *argv[])
{
#if defined(Q_OS_MAC)
// なんかうまく管理者権限が取得できないorz
//    if( acquireTaskportRight() < 0 )
//    {
//        return -1;
//    }
#endif

    QGuiApplication app(argc, argv);

    // 型の登録
    qmlRegisterType<ProcessInfo>("ProcessLib",1,0,"ProcessInfo");
    qmlRegisterType<ProcessList>("ProcessLib",1,0,"ProcessList");

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
