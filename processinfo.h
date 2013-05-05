#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <QQuickItem>

class ProcessInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(qint64 memory READ memory WRITE setMemory)

public:

    explicit ProcessInfo(QObject *parent = 0);
    explicit ProcessInfo(const ProcessInfo& rhs);
    explicit ProcessInfo(const QString& name_, qint64 memory_);

    QString name() const;
    void setName(const QString& name_);

    qint64 memory() const;
    void setMemory(qint64 memory_);

signals:
    
public slots:
    
private:
    QString m_name;
    qint64 m_memory;
};

#endif // PROCESSINFO_H
