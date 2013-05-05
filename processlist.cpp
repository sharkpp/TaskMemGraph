#include "processlist.h"

ProcessList::ProcessList(QObject *parent) :
    QObject(parent)
{
}

void ProcessList::update()
{

}

QList<QObject*> ProcessList::data() const
{
    QList<QObject*> r;
    r.append(new ProcessInfo("a", 150));
    r.append(new ProcessInfo("b", 70));
    r.append(new ProcessInfo("c", 60));
    return r;
}
