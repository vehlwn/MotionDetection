#pragma once

#include <ostream>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <utility>

extern "C" {
#include <libavutil/dict.h>
}

#include "ErrorWithContext.hpp"
#include "ffmpeg/detail/AvError.hpp"

namespace vehlwn::ffmpeg {
class ScopedAvDictionary {
    AVDictionary* m_raw = nullptr;

public:
    ScopedAvDictionary() = default;
    ScopedAvDictionary(const ScopedAvDictionary&) = delete;
    ScopedAvDictionary(ScopedAvDictionary&& rhs) noexcept
    {
        std::swap(m_raw, rhs.m_raw);
    }
    ~ScopedAvDictionary()
    {
        av_dict_free(&m_raw);
    }
    void set_str(const char* const key, const char* const value)
    {
        const int errnum = av_dict_set(&m_raw, key, value, 0);
        if(errnum < 0)
            throw ErrorWithContext("av_dict_set failed: ", detail::AvError(errnum));
    }
    AVDictionary** double_ptr()
    {
        return &m_raw;
    }

private:
    class Iterator {
        const AVDictionary* m_raw = nullptr;
        const AVDictionaryEntry* m_entry = nullptr;
        bool m_finished = false;

        auto as_tuple() const
        {
            return std::tie(m_entry, m_finished);
        }

    public:
        Iterator()
            : m_finished(true)
        {}
        explicit Iterator(AVDictionary const* const raw)
            : m_raw(raw)
        {
            ++(*this);
        }
        std::pair<std::string_view, std::string_view> operator*() const
        {
            if(m_finished)
                throw std::runtime_error("Attempt to dereference drained iterator!");
            return {m_entry->key, m_entry->value};
        }
        Iterator& operator++()
        {
            if(!m_finished) {
                m_entry = av_dict_iterate(m_raw, m_entry);
                if(!m_entry)
                    m_finished = true;
            }
            return *this;
        }
        bool operator!=(const Iterator& rhs) const
        {
            return as_tuple() != rhs.as_tuple();
        }
    };

public:
    Iterator begin() const
    {
        return Iterator(m_raw);
    }
    Iterator end() const
    {
        return Iterator();
    }
};

inline std::ostream& operator<<(std::ostream& os, const ScopedAvDictionary& dict)
{
    os << "{";
    std::string_view comma = "";
    for(const auto&& [key, val] : dict) {
        os << comma << "\"" << key << "\": \"" << val << "\"";
        comma = ", ";
    }
    os << "}";
    return os;
}
} // namespace vehlwn::ffmpeg
