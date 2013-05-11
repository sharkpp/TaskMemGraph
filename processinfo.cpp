#include "processinfo.h"

#ifndef _countof
#define _countof(a) (sizeof(a) / sizeof((a)[0]))
#endif

ProcessInfo::ProcessInfo(QObject *parent)
    : QObject(parent)
    , m_memory(0)
{
}

ProcessInfo::ProcessInfo(const ProcessInfo& rhs)
    : m_name(rhs.m_name)
    , m_memory(rhs.m_memory)
{
}

ProcessInfo::ProcessInfo(const QString& name_, qint64 memory_)
    : m_name(name_)
    , m_memory(memory_)
{
}

ProcessInfo::~ProcessInfo()
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

QString ProcessInfo::memoryDisplay() const
{
    const static struct {
        qint64 dot;
        const char* label;
    } UNIT[] = {
      //{ 1024, "B" },
        { 0x100000ULL, "KB" },
        { 0x40000000ULL, "MB" },
        { 0x10000000000ULL, "GB" },
    };

    for(size_t i = 0, num = sizeof(UNIT)/sizeof(UNIT[0]); i < num; i++)
    {
        if( m_memory < UNIT[i].dot )
        {
            qint64 dot = UNIT[i].dot / 1024;
            return QString("%1.%2%3")
                    .arg(m_memory/dot)
                    .arg(m_memory%dot*10/dot)
                    .arg(UNIT[i].label);
        }
    }

    return QString("%1B").arg(m_memory);
}
