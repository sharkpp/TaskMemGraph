#include "processinfo.h"

ProcessInfo::ProcessInfo(QObject *parent)
    : QObject(parent)
    , m_memory(0)
{
}

//ProcessInfo::ProcessInfo(const ProcessInfo& rhs)
//    : m_name(rhs.m_name)
//    , m_memory(rhs.m_memory)
//{
//}

ProcessInfo::ProcessInfo(const QString& name_, qint64 memory_)
    : m_name(name_)
    , m_memory(memory_)
{

}

QString ProcessInfo::name() const
{
    return m_name;
}

void ProcessInfo::setName(const QString& name_)
{
    m_name = name_;
}

qint64 ProcessInfo::memory() const
{
    return m_memory;
}

void ProcessInfo::setMemory(qint64 memory_)
{
    m_memory = memory_;
}
