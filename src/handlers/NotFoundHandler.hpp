#pragma once

#include "Poco/Net/HTTPRequestHandler.h"
#include "common/ContentLengthHandler.hpp"

namespace vehlwn::handlers {
class NotFoundHandler : public Poco::Net::HTTPRequestHandler {
public:
    virtual void handleRequest(
        Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override;

private:
    common::ContentLengthHandler m_content_length_handler;
};
} // namespace vehlwn::handlers
