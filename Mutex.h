#pragma once

#include <shared_mutex>

namespace vehlwn {
template<class T>
class Mutex;

namespace detail {
template<class T>
class MutexGuard
{
    friend class Mutex<T>;

private:
    MutexGuard(Mutex<T>& lock)
        : m_lock{lock}
    {
        m_lock.m_shared_mutex.lock();
    }

public:
    ~MutexGuard()
    {
        m_lock.m_shared_mutex.unlock();
    }
    T* operator->() const
    {
        return &m_lock.m_value;
    }
    T& operator*() const
    {
        return m_lock.m_value;
    }

private:
    Mutex<T>& m_lock;
};

template<class T>
class ConstMutexGuard
{
    friend class Mutex<T>;

private:
    ConstMutexGuard(const Mutex<T>& lock)
        : m_lock{lock}
    {
        m_lock.m_shared_mutex.lock_shared();
    }

public:
    ~ConstMutexGuard()
    {
        m_lock.m_shared_mutex.unlock_shared();
    }
    const T* operator->() const
    {
        return &m_lock.m_value;
    }
    const T& operator*() const
    {
        return m_lock.m_value;
    }

private:
    const Mutex<T>& m_lock;
};
}; // namespace detail

template<class T>
class Mutex
{
    friend class detail::MutexGuard<T>;
    friend class detail::ConstMutexGuard<T>;

public:
    Mutex(const Mutex&) = delete;
    Mutex(Mutex&&) = delete;
    explicit Mutex(T value)
        : m_value{std::move(value)}
    {
    }
    Mutex& operator=(const Mutex&) = delete;
    Mutex& operator=(Mutex&&) = delete;
    detail::MutexGuard<T> lock()
    {
        return detail::MutexGuard<T>{*this};
    }
    detail::ConstMutexGuard<T> lock() const
    {
        return detail::ConstMutexGuard<T>{*this};
    }

private:
    T m_value;
    mutable std::shared_mutex m_shared_mutex;
};
} // namespace vehlwn
