#include "MotionDataWorker.h"
#include "OpencvBackgroundSubtractorFactory.h"
#include "Poco/AutoPtr.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/Format.h"
#include "Poco/FormattingChannel.h"
#include "Poco/Logger.h"
#include "Poco/Message.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/PatternFormatter.h"
#include "Poco/Util/ServerApplication.h"
#include "VideoCaptureFactory.h"

#include <cstdint>
#include <iostream>
#include <memory>

class HelloRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    void handleRequest(
        Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override
    {
        response.setContentType("text/plain");

        std::ostream& ostr = response.send();
        ostr << "Hello from POCO!";
    }
};

class AppRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    Poco::Net::HTTPRequestHandler*
        createRequestHandler(const Poco::Net::HTTPServerRequest& request) override
    {
        if(request.getMethod() == "GET" && request.getURI() == "/")
            return new HelloRequestHandler;
        else
            return nullptr;
    }
};

class ServerApp : public Poco::Util::ServerApplication
{
    using base = Poco::Util::ServerApplication;

protected:
    virtual void initialize(Poco::Util::Application& self) override
    {
        const int count = loadConfiguration();
        base::initialize(self);
        auto console_channel = new Poco::ConsoleChannel;
        auto pattern_formatter = new Poco::PatternFormatter;
        pattern_formatter->setProperty(
            "pattern",
            "%Y-%m-%d %H:%M:%S [%p] [%O:%u] %t");
        auto format_channel =
            new Poco::FormattingChannel(pattern_formatter, console_channel);
        logger().setChannel(format_channel);
        logger().setLevel(Poco::Message::Priority::PRIO_TRACE);
        logger().debug("logger.debug");
        poco_information(
            logger(),
            Poco::format("Loaded %d configuration files", count));
    }

    virtual int main(const std::vector<std::string>& /*args*/) override
    {
        auto back_subtractor_factory =
            std::make_shared<vehlwn::OpencvBackgroundSubtractorFactory>(
                config(),
                logger());
        auto video_capture_factory =
            std::make_shared<vehlwn::VideoCaptureFactory>(config(), logger());
        auto motion_data_worker = std::make_shared<vehlwn::MotionDataWorker>(
            std::move(back_subtractor_factory),
            std::move(video_capture_factory),
            logger());
        motion_data_worker->start();

        auto params = new Poco::Net::HTTPServerParams;
        const std::string host_and_port =
            config().getString("http_server.host_and_port");
        Poco::Net::HTTPServer srv(
            new AppRequestHandlerFactory,
            Poco::Net::ServerSocket{Poco::Net::SocketAddress{host_and_port}},
            new Poco::Net::HTTPServerParams);
        srv.start();
        poco_information(
            logger(),
            Poco::format("Server listening %s", host_and_port));
        waitForTerminationRequest();
        srv.stop();
        return Poco::Util::Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(ServerApp)
