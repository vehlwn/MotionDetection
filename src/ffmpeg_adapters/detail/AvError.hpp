#pragma once

extern "C" {
#include <libavutil/error.h>
}

#include <array>
#include <sstream>
#include <string>

namespace vehlwn::ffmpeg::detail {
inline std::string av_error_to_string(const int errnum)
{
    std::array<char, AV_ERROR_MAX_STRING_SIZE> msg_buf = {};
    const int ret_code = av_strerror(errnum, msg_buf.data(), msg_buf.size());
    if(ret_code < 0) {
        return "Description for errnum cannot be found";
    }
    return {msg_buf.begin(), msg_buf.end()};
}

class AvError : public std::exception {
public:
    explicit AvError(const int code)
    {
        std::ostringstream os;
        os << code << ": " << detail::av_error_to_string(code);
        m_msg = os.str();
    }
    [[nodiscard]] const char* what() const noexcept override
    {
        return m_msg.c_str();
    }

private:
    std::string m_msg;
};
} // namespace vehlwn::ffmpeg::detail
