#include "NotFoundHandler.h"

namespace vehlwn::handlers {

void NotFoundHandler::handleRequest(
    Poco::Net::HTTPServerRequest& /*request*/,
    Poco::Net::HTTPServerResponse& response)
{
    response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    response.setContentType("text/plain");
    const std::string msg = "Path not found";
    m_content_length_handler.send(msg, response);
}
} // namespace vehlwn::handlers
