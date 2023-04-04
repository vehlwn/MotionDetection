#pragma once

#include <string>
#include <vector>

#include <Poco/Net/HTTPServerResponse.h>

namespace vehlwn::handlers::common {
class ContentLengthHandler {
public:
    void send(const std::string& msg, Poco::Net::HTTPServerResponse& response);
    void send(
        const std::vector<unsigned char>& msg,
        Poco::Net::HTTPServerResponse& response);
};
} // namespace vehlwn::handlers::common
