#include "MotionMaskHandler.hpp"

namespace vehlwn::handlers {

MotionMaskHandler::MotionMaskHandler(
    std::shared_ptr<const vehlwn::MotionDataWorker> motion_data_worker,
    Poco::Logger& logger)
    : m_imencode_handler{logger}
    , m_motion_data_worker{std::move(motion_data_worker)}
{
}

void MotionMaskHandler::handleRequest(
    Poco::Net::HTTPServerRequest& /*request*/,
    Poco::Net::HTTPServerResponse& response)
{
    const cv::Mat frame = m_motion_data_worker->get_motion_data()->lock()->fgmask();
    m_imencode_handler.send_encoded_image(frame, response);
}
} // namespace vehlwn::handlers
