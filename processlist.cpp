#include "processlist.h"
#include <QVector>

#if defined(Q_OS_MAC)
# include <sys/types.h>
# include <sys/sysctl.h>
# include <mach/task.h>
# include <mach/mach_init.h>
# include <mach/mach_traps.h>
#elif defined(Q_OS_WIN32)
# include <QFileInfo>
# include <windows.h>
# include <tlhelp32.h>
# include <Dbghelp.h>
# include <psapi.h>
#else
#error "not impliment1"
#endif

#ifndef _countof
#define _countof(a) (sizeof(a) / sizeof((a)[0]))
#endif

// QList<Object*>のソート時に使用
//  qSort(m_process_list.begin(),m_process_list.end(),ProcessInfoMemoryComp());
class ProcessInfoMemoryComp
{
public:
    inline bool operator()(const QObject* t1, const QObject* t2) const
    {
        const ProcessInfo* t1_ = qobject_cast<const ProcessInfo*>(t1);
        const ProcessInfo* t2_ = qobject_cast<const ProcessInfo*>(t2);
        return (t2_->memory() < t1_->memory());
    }
};

//////////////////////////////////////////////////////////
// ProcessListクラス

ProcessList::ProcessList(QObject *parent)
    : QObject(parent)
{
}

void ProcessList::update(int listNumMax, bool mergeSynonymProcess)
{
    qDeleteAll(m_process_list);
    m_process_list.clear();

    QList<QObject*> process_list;

#if defined(Q_OS_MAC)

    size_t len;

    //    int maxproc;
    //    int kern_maxproc_mib[] = { CTL_KERN, KERN_MAXPROC };
    //    len = sizeof(maxproc);
    //    if( ::sysctl(kern_maxproc_mib, _countof(kern_maxproc_mib), &maxproc, &len, NULL, 0) < 0 )
    //    {
    //        return;
    //    }

        // 動作中のプロセスのPID一覧を取得
        int kern_proc_uid_mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL };
        len = 0;
        if( ::sysctl(kern_proc_uid_mib, _countof(kern_proc_uid_mib), NULL, &len, NULL, 0) < 0 )
        {
            return;
        }
        size_t kinfo_list_num = len / sizeof(kinfo_proc);
        QVector<kinfo_proc> kinfo_procs(kinfo_list_num);
        kinfo_proc* kinfo_list_ptr = kinfo_procs.data();
        if( ::sysctl(kern_proc_uid_mib, _countof(kern_proc_uid_mib), kinfo_list_ptr, &len, NULL, 0) < 0 )
        {
            return;
        }

        // 各プロセスごとの情報を取得
        for(size_t i = 0; i < kinfo_list_num; i++)
        {
            kinfo_proc& kinfo = kinfo_list_ptr[i];
            kern_return_t kr;

            // pidからtaskに変換(要管理者権限)
            task_t task;
            kr = ::task_for_pid(mach_task_self(), kinfo.kp_proc.p_pid, &task);
            if( KERN_SUCCESS != kr )
            {
                continue;
            }

            // プロセスのメモリ使用量を取得
            struct task_basic_info_64 ti;
            mach_msg_type_number_t count = TASK_BASIC_INFO_64_COUNT;
            if( KERN_SUCCESS != ::task_info(task, TASK_BASIC_INFO_64, (task_info_t)&ti, &count) )
            {
                continue;
            }

            QString name = QString("%1").arg(kinfo.kp_proc.p_comm);
            process_list.append(new ProcessInfo(name, ti.resident_size));
        }

#elif defined(Q_OS_WIN32)

    HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if( INVALID_HANDLE_VALUE == hSnapshot )
    {
        return;
    }

    // 動作中のプロセスのPID一覧を取得
    PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
    for(BOOL    result = ::Process32First(hSnapshot, &pe);
        result; result = ::Process32Next(hSnapshot, &pe))
    {
        // 各プロセスごとの情報を取得
        HANDLE hproc = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                     FALSE, pe.th32ProcessID);
        if( INVALID_HANDLE_VALUE == hproc )
        {
            continue;
        }

        PROCESS_MEMORY_COUNTERS pmc = {};
        ::GetProcessMemoryInfo(hproc, &pmc, sizeof(pmc));

        ::CloseHandle(hproc);

        QString name = QFileInfo(QString::fromWCharArray(pe.szExeFile)).baseName();
        process_list.append(new ProcessInfo(name, pmc.WorkingSetSize));
    }

    ::CloseHandle(hSnapshot);

#else
#error "not impliment1"
#endif

    // 同じ名前のプロセスをまとめる
    if( mergeSynonymProcess )
    {
        QMap<QString, QObject*> synonym_process_list;
        for(QList<QObject*>::const_iterator
                ite = process_list.begin(),
                last= process_list.end();
            ite != last; ++ite)
        {
            const ProcessInfo* p = qobject_cast<const ProcessInfo*>(*ite);

            if( synonym_process_list.end() == synonym_process_list.find(p->name()) )
            {
                synonym_process_list[p->name()] = new ProcessInfo(p->name(), 0);
            }
            ProcessInfo* p2 = qobject_cast<ProcessInfo*>(synonym_process_list[p->name()]);
            p2->setMemory(p2->memory() + p->memory());
        }

        qDeleteAll(process_list);
        process_list.clear();

        for(QMap<QString, QObject*>::const_iterator
                ite = synonym_process_list.begin(),
                last= synonym_process_list.end();
            ite != last; ++ite)
        {
            const ProcessInfo* p = qobject_cast<const ProcessInfo*>(ite.value());
            process_list.append(new ProcessInfo(*p));
        }

        qDeleteAll(synonym_process_list);
    }

    // メモリの使用量でソート
    qSort(process_list.begin(), process_list.end(), ProcessInfoMemoryComp());

	// 多い項目をまとめる
    for(QList<QObject*>::const_iterator
            ite = process_list.begin(),
            last= process_list.end();
        ite != last; ++ite)
    {
        const ProcessInfo* p = qobject_cast<const ProcessInfo*>(*ite);

        if( listNumMax - 1 <= m_process_list.size() )
        {
            if( listNumMax - 1 == m_process_list.size() )
            {
                m_process_list.append(new ProcessInfo("[other]", 0));
            }
            ProcessInfo* p2 = qobject_cast<ProcessInfo*>(m_process_list.back());
            p2->setMemory(p2->memory() + p->memory());
        }
        else
        {
            m_process_list.append(new ProcessInfo(*p));
        }
    }

    qDeleteAll(process_list);
}

QList<QObject*> ProcessList::data() const
{
    return QList<QObject*>(m_process_list);
}
