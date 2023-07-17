#include <sstream>
#define BOOST_TEST_MODULE parser
#include <boost/test/included/unit_test.hpp>

#include "Ini.hpp"
#include "Parser.hpp"

BOOST_AUTO_TEST_CASE(ParseGood)
{
    auto in = std::istringstream(R"(
  ; comment
  # also comment

  [section]
  # in section comment
  int = 10
  double = 56.69
  str = aaa
  bool1 = true
  bool2 = false
  # after comment

  [another]
  value = 123

  # end comment
  )");
    auto val = vehlwn::ini::parser::parse(in);
    auto section_opt = val.section("section");
    BOOST_TEST(section_opt.has_value());
    auto section = section_opt.value();
    {
        const auto int_opt = section.get("int");
        BOOST_TEST(int_opt.has_value());
        const auto& int_value = int_opt.value();
        BOOST_TEST(int_value.get_number<int>() == 10);
        BOOST_TEST(int_value.get_number<double>() == 10);
    }
    {
        const auto double_opt = section.get("double");
        BOOST_TEST(double_opt.has_value());
        const auto& double_value = double_opt.value();
        BOOST_TEST(double_value.get_number<double>() == 56.69);
    }
    {
        const auto str_opt = section.get("str");
        BOOST_TEST(str_opt.has_value());
        const auto& str_value = str_opt.value();
        BOOST_TEST(str_value.get_string_view() == "aaa");
    }
    {
        const auto bool_opt = section.get("bool1");
        BOOST_TEST(bool_opt.has_value());
        const auto& bool_value = bool_opt.value();
        BOOST_TEST(bool_value.get_bool() == true);
    }
    {
        const auto bool_opt = section.get("bool2");
        BOOST_TEST(bool_opt.has_value());
        const auto& bool_value = bool_opt.value();
        BOOST_TEST(bool_value.get_bool() == false);
    }

    section = val.section("another").value();
    BOOST_TEST(section.get("value").value().get_number<int>() == 123);
}

BOOST_AUTO_TEST_CASE(EmptySection)
{
    auto in = std::istringstream("[empty section]");
    const auto val = vehlwn::ini::parser::parse(in);
    const auto section = val.section("empty section");
    BOOST_TEST(section.has_value());
}

BOOST_AUTO_TEST_CASE(EmptySectionName)
{
    auto in = std::istringstream("[]");
    const auto val = vehlwn::ini::parser::parse(in);
    const auto section = val.section("");
    BOOST_TEST(section.has_value());
}

BOOST_AUTO_TEST_CASE(EmptyValue)
{
    auto in = std::istringstream(R"(
  [  empty value  ]
  key =
  key2 =
)");
    const auto val = vehlwn::ini::parser::parse(in);
    const auto section_opt = val.section("empty value");
    BOOST_TEST(section_opt.has_value());
    const auto section = section_opt.value();

    for(const auto key_name : {"key", "key2"}) {
        auto key_opt = section.get(key_name);
        BOOST_TEST(key_opt.has_value());
        auto key = key_opt.value();
        BOOST_TEST(key.get_string_view() == "");
    }
}

BOOST_AUTO_TEST_CASE(EmptyKey)
{
    auto in = std::istringstream(R"(
  [empty key]
  = val
    )");
    const auto val = vehlwn::ini::parser::parse(in);
    const auto section_opt = val.section("empty key");
    BOOST_TEST(section_opt.has_value());
    const auto section = section_opt.value();

    auto key_opt = section.get("");
    BOOST_TEST(key_opt.has_value());
    auto key = key_opt.value();
    BOOST_TEST(key.get_string_view() == "val");
}
