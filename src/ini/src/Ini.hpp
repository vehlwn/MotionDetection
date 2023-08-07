#pragma once

#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/lexical_cast.hpp>

namespace vehlwn::ini {
using Value = std::string;
using ValueMap = std::map<std::string, Value>;
using SectionMap = std::map<std::string, ValueMap>;

class ValueWrapper {
    const Value* m_val;

public:
    explicit ValueWrapper(const Value* val)
        : m_val(val)
    {}

    [[nodiscard]] std::string_view get_string_view() const
    {
        return *m_val;
    }

    template<class T>
    T get_number() const
    {
        static_assert(
            std::is_arithmetic_v<T> && !std::is_same_v<T, bool>,
            "This method accepts only numeric types");
        return boost::lexical_cast<T>(get_string_view());
    }

    [[nodiscard]] bool get_bool() const
    {
        auto tmp = std::string(get_string_view());
        boost::algorithm::to_lower(tmp);
        if(tmp == "true" || tmp == "yes" || tmp == "on" || tmp == "1") {
            return true;
        }
        if(tmp == "false" || tmp == "no" || tmp == "off" || tmp == "0") {
            return false;
        }
        throw std::runtime_error(
            "Cannot convert '" + std::string(get_string_view()) + "' to bool");
    }
};

class Section {
    const ValueMap* m_properties;

public:
    explicit Section(const ValueMap* properties)
        : m_properties(properties)
    {}

    [[nodiscard]] std::optional<ValueWrapper> get(const std::string& key_name) const
    {
        const auto value = m_properties->find(key_name);
        if(value == m_properties->end()) {
            return std::nullopt;
        }
        return ValueWrapper(&value->second);
    }

    [[nodiscard]] const ValueMap& get_all_values() const
    {
        return *m_properties;
    }
};

class Ini {
    SectionMap m_map;

public:
    explicit Ini(SectionMap&& map)
        : m_map(std::move(map))
    {}

    [[nodiscard]] std::optional<Section>
        section(const std::string& section_name) const
    {
        auto section = m_map.find(section_name);
        if(section == m_map.end()) {
            return std::nullopt;
        }
        return Section(&section->second);
    }
};
} // namespace vehlwn::ini
