#pragma once

#include <filesystem>
#include <sstream>

#include <boost/chrono/io/time_point_io.hpp>
#include <boost/chrono/io/timezone.hpp>
#include <boost/chrono/system_clocks.hpp>

namespace vehlwn {
class FileNameFactory {
public:
    FileNameFactory() = default;
    FileNameFactory(const FileNameFactory&) = default;
    FileNameFactory(FileNameFactory&&) = default;
    virtual ~FileNameFactory() = default;
    FileNameFactory& operator=(const FileNameFactory&) = default;
    FileNameFactory& operator=(FileNameFactory&&) = default;

    virtual void set_prefix(std::string&& s) = 0;
    virtual void set_extension(std::string&& s) = 0;
    virtual std::string generate() = 0;
};

class DateFolderFactory : public FileNameFactory {
    std::string m_prefix;
    std::string m_extension;

public:
    void set_prefix(std::string&& s) override
    {
        m_prefix = std::move(s);
    }
    void set_extension(std::string&& s) override
    {
        m_extension = std::move(s);
    }
    std::string generate() override
    {
        const auto now = boost::chrono::system_clock::now();
        std::ostringstream os;
        using boost::chrono::time_fmt, boost::chrono::timezone;
        os << time_fmt(timezone::local, "%Y-%m-%d") << now;
        const auto date_component = os.str();
        os.str("");
        os << time_fmt(timezone::local, "%H.%M.%S") << now;
        const auto time_component = os.str();

        std::filesystem::path ret = m_prefix;
        ret /= date_component;
        std::filesystem::create_directories(ret);
        ret /= time_component;
        ret += m_extension;
        return ret.string();
    }
};

} // namespace vehlwn
