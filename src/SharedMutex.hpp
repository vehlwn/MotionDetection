#pragma once

#include <mutex>
#include <shared_mutex>

namespace vehlwn {
template<class T>
class SharedMutex;

namespace detail {
template<class T>
class MutexGuard {
    friend class SharedMutex<T>;

private:
    MutexGuard(SharedMutex<T>& mutex)
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
    SharedMutex<T>& m_mutex;
    std::unique_lock<std::shared_mutex> m_lock;
};

template<class T>
class ConstMutexGuard {
    friend class SharedMutex<T>;

private:
    ConstMutexGuard(const SharedMutex<T>& mutex)
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
    const SharedMutex<T>& m_mutex;
    std::shared_lock<std::shared_mutex> m_lock;
};
} // namespace detail

template<class T>
class SharedMutex {
    friend class detail::MutexGuard<T>;
    friend class detail::ConstMutexGuard<T>;

public:
    SharedMutex()
        : m_value()
    {}
    explicit SharedMutex(T&& value)
        : m_value(std::move(value))
    {}
    SharedMutex(const SharedMutex&) = delete;
    SharedMutex(SharedMutex&&) = delete;
    SharedMutex& operator=(const SharedMutex&) = delete;
    SharedMutex& operator=(SharedMutex&&) = delete;
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
