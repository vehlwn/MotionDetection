#pragma once

#include <functional>
#include <map>
#include <memory>
#include <tuple>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>

#include "MotionDataWorker.hpp"

namespace vehlwn {
class AppRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:
    AppRequestHandlerFactory(
        std::shared_ptr<vehlwn::MotionDataWorker>&& motion_data_worker);
    ~AppRequestHandlerFactory();
    virtual Poco::Net::HTTPRequestHandler*
        createRequestHandler(const Poco::Net::HTTPServerRequest& request) override;

private:
    std::shared_ptr<vehlwn::MotionDataWorker> m_motion_data_worker;
    std::map<
        std::tuple<std::string, std::string>,
        std::function<Poco::Net::HTTPRequestHandler*()>>
        m_routes;
};
} // namespace vehlwn
