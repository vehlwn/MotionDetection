#pragma once

#include <mutex>
#include <shared_mutex>

namespace vehlwn {
template<class T>
class Mutex;

namespace detail {
template<class T>
class MutexGuard {
    friend class Mutex<T>;

private:
    MutexGuard(Mutex<T>& mutex)
        : m_mutex{mutex}
        , m_lock{m_mutex.m_inner}
    {}

public:
    T* operator->() const
    {
        return &m_mutex.m_value;
    }
    T& operator*() const
    {
        return m_mutex.m_value;
    }

private:
    Mutex<T>& m_mutex;
    std::unique_lock<std::shared_mutex> m_lock;
};

template<class T>
class ConstMutexGuard {
    friend class Mutex<T>;

private:
    ConstMutexGuard(const Mutex<T>& mutex)
        : m_mutex{mutex}
        , m_lock{m_mutex.m_inner}
    {}

public:
    const T* operator->() const
    {
        return &m_mutex.m_value;
    }
    const T& operator*() const
    {
        return m_mutex.m_value;
    }

private:
    const Mutex<T>& m_mutex;
    std::shared_lock<std::shared_mutex> m_lock;
};
} // namespace detail

template<class T>
class Mutex {
    friend class detail::MutexGuard<T>;
    friend class detail::ConstMutexGuard<T>;

public:
    Mutex()
        : m_value{}
    {}
    explicit Mutex(T value)
        : m_value{std::move(value)}
    {}
    Mutex(const Mutex&) = delete;
    Mutex(Mutex&&) = delete;
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
    mutable std::shared_mutex m_inner;
};
} // namespace vehlwn
