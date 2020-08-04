#pragma once

#include <QObject>
#include <QTimer>

class TimerWorker : public QObject
{
    Q_OBJECT

public:
    TimerWorker(int msec, Qt::TimerType atype = Qt::CoarseTimer)
    {
        connect(&m_timer, &QTimer::timeout, this, &TimerWorker::onTimeout);
        m_timer.setInterval(msec);
        m_timer.setTimerType(atype);
        m_timer.start();
    }

protected slots:
    virtual void onTimeout() = 0;

private:
    QTimer m_timer;
};
