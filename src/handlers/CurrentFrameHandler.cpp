#include "CurrentFrameHandler.hpp"

namespace vehlwn::handlers {

CurrentFrameHandler::CurrentFrameHandler(
    std::shared_ptr<const vehlwn::MotionDataWorker>&& motion_data_worker)
    : m_motion_data_worker(std::move(motion_data_worker))
{}

void CurrentFrameHandler::handleRequest(
    Poco::Net::HTTPServerRequest& /*request*/,
    Poco::Net::HTTPServerResponse& response)
{
    cv::Mat frame;
    {
        const auto lock = m_motion_data_worker->get_motion_data()->lock();
        frame = lock->frame().get().clone();
    }
    m_imencode_handler.send_encoded_image(frame, response);
}

} // namespace vehlwn::handlers
