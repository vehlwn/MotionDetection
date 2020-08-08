#pragma once

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <cstddef>
#include <deque>
#include <optional>

template<class T>
class FixedThreadSafeQueue
{
public:
    using value_type = T;

    FixedThreadSafeQueue(const std::size_t maxSize = 128)
        : m_maxSize{maxSize}
        , m_stopped{false}
    {
    }

    FixedThreadSafeQueue(const FixedThreadSafeQueue&) = delete;
    FixedThreadSafeQueue(FixedThreadSafeQueue&&) = delete;
    FixedThreadSafeQueue& operator=(const FixedThreadSafeQueue&) = delete;
    FixedThreadSafeQueue& operator=(FixedThreadSafeQueue&&) = delete;

    ~FixedThreadSafeQueue()
    {
        stop();
    }

    void waitPush(value_type x)
    {
        QMutexLocker lock{&m_mutex};
        while(m_q.size() == m_maxSize && !m_stopped)
            m_condvar.wait(&m_mutex);
        if(m_stopped)
            return;
        m_q.push_back(std::move(x));
        m_condvar.wakeOne();
    }

    std::optional<value_type> waitPop()
    {
        QMutexLocker lock{&m_mutex};
        while(m_q.empty() && !m_stopped)
            m_condvar.wait(&m_mutex);
        if(m_stopped)
            return std::nullopt;
        value_type ret = std::move(m_q.front());
        m_q.pop_front();
        m_condvar.wakeOne();
        return std::make_optional(std::move(ret));
    }

    void stop()
    {
        QMutexLocker lock{&m_mutex};
        m_stopped = true;
        m_condvar.wakeAll();
        m_q.clear();
    }

    void start()
    {
        QMutexLocker lock{&m_mutex};
        m_stopped = false;
    }

    std::size_t size() const
    {
        QMutexLocker lock{&m_mutex};
        return m_q.size();
    }

private:
    std::size_t m_maxSize;
    std::deque<value_type> m_q;
    mutable QMutex m_mutex;
    QWaitCondition m_condvar;
    bool m_stopped;
};
