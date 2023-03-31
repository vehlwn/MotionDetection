#include <ostream>

extern "C" {
#include <libavutil/rational.h>
}

inline std::ostream& operator<<(std::ostream& os, const AVRational x)
{
    os << x.num << '/' << x.den;
    return os;
}
