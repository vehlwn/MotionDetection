#include "FpsHandler.h"

#include "fmt/core.h"

namespace vehlwn::handlers {
FpsHandler ::FpsHandler(
    std::shared_ptr<const vehlwn::MotionDataWorker> motion_data_worker)
    : m_motion_data_worker{std::move(motion_data_worker)}
{
}

void FpsHandler ::handleRequest(
    Poco::Net::HTTPServerRequest& /*request*/,
    Poco::Net::HTTPServerResponse& response)
{
    const double fps = m_motion_data_worker->get_fps();
    const std::string msg = fmt::format("{}", fps);
    response.setContentType("text/plain");
    m_content_length_handler.send(msg, response);
}

} // namespace vehlwn::handlers
