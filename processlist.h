#ifndef PROCESSLIST_H
#define PROCESSLIST_H

#include <QQuickItem>
#include <QList>
#include "processinfo.h"

class ProcessList : public QObject
{
    Q_OBJECT
public:
    explicit ProcessList(QObject *parent = 0);

    Q_INVOKABLE void update();
    Q_INVOKABLE QList<QObject*> data() const;
signals:
    
public slots:
    
};

#endif // PROCESSLIST_H
