#include "AppRequestHandlerFactory.h"
#include "MotionDataWorker.h"
#include "OpencvBackgroundSubtractorFactory.h"
#include "Poco/AutoPtr.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/Logger.h"
#include "Poco/Message.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/PatternFormatter.h"
#include "Poco/Util/ServerApplication.h"
#include "VideoCaptureFactory.h"
#include "fmt/core.h"

#include <iostream>
#include <memory>

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
            fmt::format("Loaded {} configuration files", count));
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

        const std::string host_and_port =
            config().getString("http_server.host_and_port");
        Poco::Net::HTTPServer srv{
            new vehlwn::AppRequestHandlerFactory{
                std::move(motion_data_worker),
                logger()},
            Poco::Net::ServerSocket{Poco::Net::SocketAddress{host_and_port}},
            new Poco::Net::HTTPServerParams};
        srv.start();
        poco_information(
            logger(),
            fmt::format("Server listening {}", host_and_port));

        waitForTerminationRequest();
        srv.stop();
        return Poco::Util::Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(ServerApp)
