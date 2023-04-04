#pragma once

#include <Poco/Net/HTTPRequestHandler.h>

#include "MotionDataWorker.hpp"
#include "common/ContentLengthHandler.hpp"

namespace vehlwn::handlers {
class FpsHandler : public Poco::Net::HTTPRequestHandler {
public:
    FpsHandler(std::shared_ptr<const vehlwn::MotionDataWorker>&& motion_data_worker);
    virtual void handleRequest(
        Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override;

private:
    std::shared_ptr<const vehlwn::MotionDataWorker> m_motion_data_worker;
    common::ContentLengthHandler m_content_length_handler;
};
} // namespace vehlwn::handlers
