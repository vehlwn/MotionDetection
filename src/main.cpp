#include <iostream>
#include <memory>

#include <Poco/AutoPtr.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Logger.h>
#include <Poco/Message.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/PatternFormatter.h>
#include <Poco/Util/ServerApplication.h>
#include <PreprocessImageFactory.hpp>
#include <fmt/core.h>

#include "AppRequestHandlerFactory.hpp"
#include "ApplicationSettings.hpp"
#include "BackgroundSubtractorFactory.hpp"
#include "DemuxerOptionsFactory.hpp"
#include "FfmpegInputDeviceFactory.hpp"
#include "FileNameFactory.hpp"
#include "MotionDataWorker.hpp"

class ServerApp : public Poco::Util::ServerApplication {
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
            "%Y-%m-%d %H:%M:%S.%F [%p] [tid=%I] [%O:%u] %t");
        auto format_channel
            = new Poco::FormattingChannel(pattern_formatter, console_channel);
        logger().setChannel(format_channel);
        logger().setLevel(Poco::Message::Priority::PRIO_TRACE);
        logger().debug("logger.debug");
        poco_information(
            logger(),
            fmt::format("Loaded {} configuration files", count));
    }

    virtual int main(const std::vector<std::string>& /*args*/) override
    {
        const auto application_settings
            = std::make_shared<const vehlwn::ApplicationSettings>(
                vehlwn::read_settings(config(), logger()));
        auto back_subtractor_factory
            = std::make_shared<vehlwn::BackgroundSubtractorFactory>(
                application_settings->background_subtractor,
                logger());

        auto input_device
            = vehlwn::FfmpegInputDeviceFactory(*application_settings).create();

        auto out_filename_factory = std::make_shared<vehlwn::DateFolderFactory>();
        out_filename_factory->set_prefix(
            std::string(application_settings->output_files.prefix));
        out_filename_factory->set_extension(
            std::string(application_settings->output_files.extension));

        auto preprocess_image_factory
            = std::make_shared<vehlwn::PreprocessImageFactory>(
                application_settings->preprocess,
                logger());
        auto motion_data_worker = std::make_shared<vehlwn::MotionDataWorker>(
            std::move(back_subtractor_factory),
            std::move(input_device),
            std::move(out_filename_factory),
            std::move(preprocess_image_factory),
            logger());
        motion_data_worker->start();

        const std::string host_and_port
            = config().getString("http_server.host_and_port");
        Poco::Net::HTTPServer srv{
            new vehlwn::AppRequestHandlerFactory{motion_data_worker, logger()},
            Poco::Net::ServerSocket{application_settings->http_server_host_and_port},
            new Poco::Net::HTTPServerParams};
        srv.start();
        poco_information(
            logger(),
            fmt::format(
                "Server listening {}",
                application_settings->http_server_host_and_port.toString()));

        waitForTerminationRequest();
        srv.stopAll(true);

        // We must join working thread here because motion_data_worker can
        // somehow overlive this main() method and cause segfault later
        // somewhere in opencv.
        motion_data_worker->stop();
        poco_information(
            logger(),
            fmt::format(
                "motion_data_worker.use_count = {}",
                motion_data_worker.use_count()));
        return Poco::Util::Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(ServerApp)
