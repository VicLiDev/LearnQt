#include "worker.h"
#include <QThread>

Worker::Worker(QObject *parent)
    : QObject(parent)
    , m_abort(false)
{
}

Worker::~Worker()
{
}

void Worker::doWork(int totalSteps)
{
    m_abort = false;

    for (int i = 0; i <= totalSteps; ++i) {
        if (m_abort) {
            break;
        }

        emit progress(i);
        QThread::msleep(50);
    }

    emit finished();
}
