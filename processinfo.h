#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <QQuickItem>

class ProcessInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(qint64 memory READ memory WRITE setMemory)
    Q_PROPERTY(QString memoryDisplay READ memoryDisplay)

public:

    explicit ProcessInfo(QObject *parent = 0);
    ProcessInfo(const ProcessInfo& rhs);
    ProcessInfo(const QString& name_, qint64 memory_);
    virtual ~ProcessInfo();

    QString name() const;
    void setName(const QString& name_);

    qint64 memory() const;
    void setMemory(qint64 memory_);

    QString memoryDisplay() const;

signals:
    
public slots:
    
private:
    QString m_name;
    qint64 m_memory;
};

#endif // PROCESSINFO_H
