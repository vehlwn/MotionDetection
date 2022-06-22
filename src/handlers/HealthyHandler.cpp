#include "HealthyHandler.h"

namespace vehlwn::handlers {
void HealthyHandler ::handleRequest(
    Poco::Net::HTTPServerRequest& /*request*/,
    Poco::Net::HTTPServerResponse& response)
{
    m_ok_handler.send(response);
}

} // namespace vehlwn::handlers
