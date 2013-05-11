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

    Q_INVOKABLE void update(int listNumMax, bool mergeSynonymProcess);
    Q_INVOKABLE QList<QObject *> data() const;

signals:
    
public slots:

private:
    QList<QObject*> m_process_list;
};

#endif // PROCESSLIST_H
