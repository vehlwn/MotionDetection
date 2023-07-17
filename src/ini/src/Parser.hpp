#pragma once

#include <istream>

#include "Ini.hpp"

namespace vehlwn::ini::parser {
Ini parse(std::istream& s);

} // namespace vehlwn::ini::parser
