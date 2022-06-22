#include "OkHandler.hpp"

namespace vehlwn::handlers::common {
void OkHandler::send(Poco::Net::HTTPServerResponse& response) {
    response.setContentType("text/plain");
    m_content_length_handler.send("ok", response);
}
} // namespace vehlwn::handlers::common
