#pragma once

#include "../MotionDataWorker.hpp"
#include "Poco/Net/HTTPRequestHandler.h"
#include "common/ImencodeHandler.hpp"

namespace vehlwn::handlers {

class MotionMaskHandler : public Poco::Net::HTTPRequestHandler {
public:
    MotionMaskHandler(
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
