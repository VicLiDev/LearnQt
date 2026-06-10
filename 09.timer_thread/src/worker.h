#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

public slots:
    void doWork(int totalSteps);

signals:
    void progress(int value);
    void finished();

private:
    bool m_abort;
};

#endif // WORKER_H
