#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace vehlwn {
class ErrorWithContext : public std::runtime_error {
    using base = std::runtime_error;

public:
    ErrorWithContext(std::string&& context, const std::exception& rhs)
        : base(gen_message(std::move(context), rhs))
    {}

private:
    static std::string gen_message(std::string&& context, const std::exception& rhs)
    {
        context += "\nCaused by: ";
        context += rhs.what();
        return context;
    }
};

template<class F, class ErrorStringGen>
auto invoke_with_error_context_fun(F&& f, ErrorStringGen&& g)
    -> std::invoke_result_t<F>
try {
    return f();
} catch(const std::exception& ex) {
    throw ErrorWithContext(g(), ex);
}

template<class F>
auto invoke_with_error_context_str(F&& f, std::string&& context)
    -> std::invoke_result_t<F>
{
    return invoke_with_error_context_fun(
        std::forward<F>(f),
        [context = std::move(context)] { return context; });
}
} // namespace vehlwn
