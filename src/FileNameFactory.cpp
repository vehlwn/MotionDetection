#include "FileNameFactory.hpp"

#include <filesystem>
#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace vehlwn {
namespace {
std::string
    format_ptime(const boost::posix_time::ptime& ptime, const char* const format)
{
    auto facet = new boost::posix_time::time_facet(format);
    auto os = std::ostringstream();
    os.imbue(std::locale(os.getloc(), facet));
    os << ptime;
    return os.str();
}

std::string get_date_component(const boost::posix_time::ptime& ptime)
{
    return format_ptime(ptime, "%Y-%m-%d");
}

std::string get_time_component(const boost::posix_time::ptime& ptime)
{
    return format_ptime(ptime, "%H.%M.%S");
}

} // namespace

std::string DateFolderFactory::generate()
{
    const auto now = boost::posix_time::microsec_clock::local_time();
    const auto date_component = get_date_component(now);
    const auto time_component = get_time_component(now);

    auto ret = std::filesystem::path(m_prefix);
    ret /= date_component;
    std::filesystem::create_directories(ret);
    ret /= time_component;
    ret += m_extension;
    return ret.string();
}
} // namespace vehlwn
