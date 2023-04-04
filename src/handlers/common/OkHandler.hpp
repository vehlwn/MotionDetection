#pragma once

#include <Poco/Net/HTTPServerResponse.h>

#include "ContentLengthHandler.hpp"

namespace vehlwn::handlers::common {
class OkHandler {
public:
    void send(Poco::Net::HTTPServerResponse& response);

private:
    ContentLengthHandler m_content_length_handler;
};

} // namespace vehlwn::handlers::common
