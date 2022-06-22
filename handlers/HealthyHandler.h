#pragma once

#include "Poco/Net/HTTPRequestHandler.h"
#include "common/OkHandler.h"

namespace vehlwn::handlers {
class HealthyHandler : public Poco::Net::HTTPRequestHandler
{
public:
    virtual void handleRequest(
        Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override;

private:
    common::OkHandler m_ok_handler;
};
} // namespace vehlwn::handlers
