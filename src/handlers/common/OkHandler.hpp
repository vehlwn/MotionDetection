#pragma once

#include "ContentLengthHandler.hpp"
#include "Poco/Net/HTTPServerResponse.h"

namespace vehlwn::handlers::common {
class OkHandler
{
public:
    void send(Poco::Net::HTTPServerResponse& response);

private:
    ContentLengthHandler m_content_length_handler;
};

} // namespace vehlwn::handlers::common
