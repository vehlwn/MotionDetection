#include "Parser.hpp"

#include <stdexcept>
#include <string>
#include <utility>

#include "Ini.hpp"
#include <boost/algorithm/string/trim.hpp>

namespace vehlwn::ini::parser {

namespace {
class Parser {
    std::istream& m_s;

public:
    explicit Parser(std::istream& s)
        : m_s(s)
    {}

    ini::SectionMap parse()
    {
        int line_no = 0;
        auto ret = ini::SectionMap();
        auto current_section = ret.end();
        auto line = std::string();
        while(m_s.good()) {
            line_no++;
            std::getline(m_s, line);
            if(!m_s.good() && !m_s.eof()) {
                throw std::runtime_error(
                    "Read error on line " + std::to_string(line_no));
            }
            boost::algorithm::trim(line);
            if(line.empty()) {
                continue;
            }
            if(line[0] == ';' || line[0] == '#') {
                // comment
                continue;
            }
            if(line[0] == '[') {
                // section header
                const auto end = line.find(']');
                if(end == std::string::npos) {
                    throw std::runtime_error(
                        "Unmatched [ on line " + std::to_string(line_no));
                }
                auto section_name = line.substr(1, end - 1);
                boost::algorithm::trim(section_name);
                current_section
                    = ret.emplace(std::move(section_name), ValueMap{}).first;
            } else {
                // section body
                const auto eq_pos = line.find('=');
                if(eq_pos == std::string::npos) {
                    throw std::runtime_error(
                        "'=' not found on line " + std::to_string(line_no));
                }
                auto key = line.substr(0, eq_pos);
                boost::algorithm::trim(key);
                auto value = line.substr(eq_pos + 1, std::string::npos);
                boost::algorithm::trim(value);
                if(current_section == ret.end()) {
                    throw std::runtime_error(
                        "Attempt to insert key to a nonexistent section on line"
                        + std::to_string(line_no));
                }
                current_section->second.emplace(std::move(key), std::move(value));
            }
        }
        return ret;
    }
};
} // namespace

Ini parse(std::istream& s)
{
    auto p = Parser(s);
    return Ini(p.parse());
}
} // namespace vehlwn::ini::parser
