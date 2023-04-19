#pragma once

#include <stdexcept>
#include <string>

namespace vehlwn {
class ErrorWithContext : public std::exception {
public:
    ErrorWithContext(std::string&& context, const std::exception& rhs)
        : m_msg(std::move(context += rhs.what()))
    {}
    [[nodiscard]] const char* what() const noexcept override
    {
        return m_msg.c_str();
    }

private:
    std::string m_msg;
};
} // namespace vehlwn
