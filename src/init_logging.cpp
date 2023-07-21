#include "init_logging.hpp"
#include "ApplicationSettings.hpp"

#include <iostream>

#include <boost/core/null_deleter.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/expressions/formatters/named_scope.hpp>
#include <boost/log/expressions/formatters/stream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/smart_ptr/make_shared.hpp>

extern "C" {
#include <libavutil/log.h>
}

namespace vehlwn {
namespace {
void init_boost_log(const ApplicationSettings::Logging& logging)
{
    auto core = boost::log::core::get();
    auto backend = boost::make_shared<boost::log::sinks::text_ostream_backend>();
    backend->add_stream(
        boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter{}));
    backend->auto_flush(true);

    auto sink = boost::make_shared<boost::log::sinks::synchronous_sink<
        boost::log::sinks::text_ostream_backend>>(backend);
    core->add_sink(sink);

    namespace expr = boost::log::expressions;
    namespace attrs = boost::log::attributes;
    namespace keywords = boost::log::keywords;

#define COMMON_FORMAT                                                               \
    "[" << boost::log::trivial::severity << "] ["                                   \
        << expr::format_named_scope(                                                \
               "Scope",                                                             \
               keywords::format = "%F:%l",                                          \
               keywords::depth = 1,                                                 \
               keywords::incomplete_marker = "")                                    \
        << "] " << expr::smessage
    if(logging.show_timestamp) {
        sink->set_formatter(
            expr::stream << "["
                         << expr::format_date_time<attrs::local_clock::value_type>(
                                "TimeStamp",
                                "%Y-%m-%d %H:%M:%S.%f")
                         << "] " << COMMON_FORMAT);
    } else {
        sink->set_formatter(expr::stream << COMMON_FORMAT);
    }
#undef COMMON_FORMAT

    if(logging.show_timestamp) {
        core->add_global_attribute("TimeStamp", attrs::local_clock());
    }
    core->add_global_attribute("Scope", attrs::named_scope());
    auto level = boost::log::trivial::info;
    const auto& log_level = logging.app_level;
    if(log_level == "trace") {
        level = boost::log::trivial::trace;
    } else if(log_level == "debug") {
        level = boost::log::trivial::debug;
    } else if(log_level == "info") {
        level = boost::log::trivial::info;
    } else if(log_level == "warning") {
        level = boost::log::trivial::warning;
    } else if(log_level == "error") {
        level = boost::log::trivial::error;
    } else if(log_level == "fatal") {
        level = boost::log::trivial::fatal;
    }
    core->set_filter(boost::log::trivial::severity >= level);
}

void init_ffmpeg_log(const std::string_view log_level)
{
    auto level = AV_LOG_INFO;
    if(log_level == "trace") {
        level = AV_LOG_TRACE;
    } else if(log_level == "debug") {
        level = AV_LOG_DEBUG;
    } else if(log_level == "info") {
        level = AV_LOG_INFO;
    } else if(log_level == "warning") {
        level = AV_LOG_WARNING;
    } else if(log_level == "error") {
        level = AV_LOG_ERROR;
    } else if(log_level == "fatal") {
        level = AV_LOG_FATAL;
    }
    av_log_set_level(level);
}
} // namespace

void init_logging(const ApplicationSettings::Logging& logging)
{
    init_boost_log(logging);
    init_ffmpeg_log(logging.ffmpeg_level);
}
} // namespace vehlwn
