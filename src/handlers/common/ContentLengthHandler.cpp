#include "ContentLengthHandler.hpp"

namespace vehlwn::handlers::common {
void ContentLengthHandler::send(
    const std::string& msg,
    Poco::Net::HTTPServerResponse& response)
{
    response.sendBuffer(reinterpret_cast<const void*>(msg.data()), msg.size());
}

void ContentLengthHandler::send(
    const std::vector<unsigned char>& msg,
    Poco::Net::HTTPServerResponse& response)
{
    response.sendBuffer(reinterpret_cast<const void*>(msg.data()), msg.size());
}
} // namespace vehlwn::handlers::common
