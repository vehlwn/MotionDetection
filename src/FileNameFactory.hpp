#pragma once

#include <string>

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
    std::string generate() override;
};

} // namespace vehlwn
