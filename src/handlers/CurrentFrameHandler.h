#pragma once

#include "../MotionDataWorker.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "common/ImencodeHandler.h"

namespace vehlwn::handlers {

class CurrentFrameHandler : public Poco::Net::HTTPRequestHandler
{
public:
    CurrentFrameHandler(
        std::shared_ptr<const vehlwn::MotionDataWorker> motion_data_worker,
        Poco::Logger& logger);
    virtual void handleRequest(
        Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override;

private:
    common::ImencodeHandler m_imencode_handler;
    std::shared_ptr<const vehlwn::MotionDataWorker> m_motion_data_worker;
};
} // namespace vehlwn::handlers
