#pragma once

#include <mutex>
#include <shared_mutex>

namespace vehlwn {
template<class T>
class SharedMutex;

namespace detail {
template<class T>
class WriteMutexGuard {
    friend class SharedMutex<T>;

private:
    explicit WriteMutexGuard(const SharedMutex<T>& mutex)
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
    const SharedMutex<T>& m_mutex;
    std::unique_lock<std::shared_mutex> m_lock;
};

template<class T>
class ReadMutexGuard {
    friend class SharedMutex<T>;

private:
    explicit ReadMutexGuard(const SharedMutex<T>& mutex)
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
    friend class detail::WriteMutexGuard<T>;
    friend class detail::ReadMutexGuard<T>;

public:
    SharedMutex()
        : m_value()
    {}
    explicit SharedMutex(T&& value)
        : m_value(std::move(value))
    {}
    SharedMutex(const SharedMutex&) = delete;
    SharedMutex(SharedMutex&&) = delete;
    ~SharedMutex() = default;
    SharedMutex& operator=(const SharedMutex&) = delete;
    SharedMutex& operator=(SharedMutex&&) = delete;
    detail::WriteMutexGuard<T> write() const
    {
        return detail::WriteMutexGuard<T>{*this};
    }
    detail::ReadMutexGuard<T> read() const
    {
        return detail::ReadMutexGuard<T>{*this};
    }

private:
    mutable T m_value;
    mutable std::shared_mutex m_inner;
};
} // namespace vehlwn
