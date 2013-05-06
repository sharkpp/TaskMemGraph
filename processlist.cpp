#include "processlist.h"
#include <QVector>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/task.h>
#include <mach/mach_init.h>
#include <mach/mach_traps.h>

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

void ProcessList::update(int listNumMax)
{
    qDeleteAll(m_process_list);
    m_process_list.clear();

    QList<QObject*> process_list;

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

        mach_port_name_t mpn = mach_task_self();
        task_t task;
        kr = ::task_for_pid(mpn, kinfo.kp_proc.p_pid, &task);
        if( KERN_SUCCESS != kr )
        {
            continue;
        }

        struct task_basic_info_64 ti;
        mach_msg_type_number_t count = TASK_BASIC_INFO_64_COUNT;
        if( KERN_SUCCESS != ::task_info(task, TASK_BASIC_INFO_64, (task_info_t)&ti, &count) )
        {
            continue;
        }


        QString name = QString("%1").arg(kinfo.kp_proc.p_comm);
        process_list.append(new ProcessInfo(name, ti.resident_size));
    }

    // メモリの使用量でソート
    qSort(process_list.begin(), process_list.end(), ProcessInfoMemoryComp());

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


}

QList<QObject*> ProcessList::data() const
{
    QList<QObject*> r(m_process_list);
//    for(QList<QObject*>::const_iterator
//            ite = m_process_list.begin(),
//            last= m_process_list.end();
//        ite != last; ++ite)
//    {
//        const ProcessInfo* p = qobject_cast<const ProcessInfo*>(*ite);
//        r.append(new ProcessInfo(*p));
//    }
    return r;
}
