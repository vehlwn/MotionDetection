#include "AppRequestHandlerFactory.hpp"

#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/URI.h>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>

#include "handlers/CurrentFrameHandler.hpp"
#include "handlers/FpsHandler.hpp"
#include "handlers/HealthyHandler.hpp"
#include "handlers/IndexHandler.hpp"
#include "handlers/MotionMaskHandler.hpp"
#include "handlers/NotFoundHandler.hpp"

namespace vehlwn {
AppRequestHandlerFactory::AppRequestHandlerFactory(
    std::shared_ptr<vehlwn::MotionDataWorker>&& motion_data_worker)
    : m_motion_data_worker(std::move(motion_data_worker))
{
    BOOST_LOG_FUNCTION();
    BOOST_LOG_TRIVIAL(debug) << "constructor AppRequestHandlerFactory";
    auto motion_data_worker_copy = m_motion_data_worker;
    m_routes[{"GET", "/api/healthy"}] = [] { return new handlers::HealthyHandler; };
    m_routes[{"GET", "/api/current_frame"}]
        = [=] { return new handlers::CurrentFrameHandler(motion_data_worker_copy); };
    m_routes[{"GET", "/api/motion_mask"}]
        = [=] { return new handlers::MotionMaskHandler(motion_data_worker_copy); };
    m_routes[{"GET", "/api/fps"}]
        = [=] { return new handlers::FpsHandler{motion_data_worker_copy}; };
    m_routes[{"GET", "/front/index.html"}]
        = [] { return new handlers::IndexHandler; };
}

AppRequestHandlerFactory::~AppRequestHandlerFactory()
{
    BOOST_LOG_FUNCTION();
    BOOST_LOG_TRIVIAL(debug) << "destructor ~AppRequestHandlerFactory";
}

Poco::Net::HTTPRequestHandler* AppRequestHandlerFactory::createRequestHandler(
    const Poco::Net::HTTPServerRequest& request)
{
    BOOST_LOG_FUNCTION();
    BOOST_LOG_TRIVIAL(info) << request.clientAddress().toString() << " \""
                            << request.getMethod() << "\" " << request.getURI()
                            << "\" \""
                            << (request.has("user-agent") ? request.get("user-agent")
                                                          : "-");
    const auto url = Poco::URI(request.getURI());
    if(const auto it = m_routes.find(std::tie(request.getMethod(), url.getPath()));
       it != m_routes.end())
        return it->second();
    else
        return new handlers::NotFoundHandler;
}
} // namespace vehlwn
