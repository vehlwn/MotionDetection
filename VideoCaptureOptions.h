#pragma once

#include <QString>
#include <variant>

struct VideoCaptureOptions
{
    std::variant<QString, int> fname;
};
