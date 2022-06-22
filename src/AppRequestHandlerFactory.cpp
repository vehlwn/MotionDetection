#include "AppRequestHandlerFactory.hpp"

#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/URI.h"
#include "fmt/core.h"
#include "handlers/CurrentFrameHandler.hpp"
#include "handlers/FpsHandler.hpp"
#include "handlers/HealthyHandler.hpp"
#include "handlers/IndexHandler.hpp"
#include "handlers/MotionMaskHandler.hpp"
#include "handlers/NotFoundHandler.hpp"

#include <Poco/Logger.h>
#include <vector>

namespace vehlwn {
AppRequestHandlerFactory::AppRequestHandlerFactory(
    std::shared_ptr<vehlwn::MotionDataWorker> motion_data_worker,
    Poco::Logger& logger)
    : m_motion_data_worker{std::move(motion_data_worker)}
    , m_logger{logger}
{
    poco_information(m_logger, "constructor AppRequestHandlerFactory");
    auto* logger_copy = &m_logger;
    auto motion_data_worker_copy = m_motion_data_worker;
    m_routes[{"GET", "/api/healthy"}] = [] { return new handlers::HealthyHandler; };
    m_routes[{"GET", "/api/current_frame"}] = [=] {
        return new handlers::CurrentFrameHandler{
            motion_data_worker_copy,
            *logger_copy};
    };
    m_routes[{"GET", "/api/motion_mask"}] = [=] {
        return new handlers::MotionMaskHandler{
            motion_data_worker_copy,
            *logger_copy};
    };
    m_routes[{"GET", "/api/fps"}]
        = [=] { return new handlers::FpsHandler{motion_data_worker_copy}; };
    m_routes[{"GET", "/front/index.html"}]
        = [] { return new handlers::IndexHandler; };
}

AppRequestHandlerFactory::~AppRequestHandlerFactory()
{
    poco_information(m_logger, "destructor ~AppRequestHandlerFactory");
}

Poco::Net::HTTPRequestHandler* AppRequestHandlerFactory::createRequestHandler(
    const Poco::Net::HTTPServerRequest& request)
{
    poco_information(
        m_logger,
        fmt::format(
            "{remote_addr} \"{method} {uri}\" \"{http_user_agent}\"",
            fmt::arg("remote_addr", request.clientAddress().toString()),
            fmt::arg("method", request.getMethod()),
            fmt::arg("uri", request.getURI()),
            fmt::arg(
                "http_user_agent",
                request.has("User-Agent") ? request.get("User-Agent") : "-")));
    const Poco::URI url{request.getURI()};
    if(const auto it = m_routes.find(std::tie(request.getMethod(), url.getPath()));
       it != m_routes.end())
        return it->second();
    else
        return new handlers::NotFoundHandler;
}
} // namespace vehlwn
