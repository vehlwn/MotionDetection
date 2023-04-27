#include <iostream>
#include <memory>

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Util/ServerApplication.h>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>

#include "AppRequestHandlerFactory.hpp"
#include "ApplicationSettings.hpp"
#include "BackgroundSubtractorFactory.hpp"
#include "DemuxerOptionsFactory.hpp"
#include "FfmpegInputDeviceFactory.hpp"
#include "FileNameFactory.hpp"
#include "MotionDataWorker.hpp"
#include "PreprocessImageFactory.hpp"
#include "init_logging.hpp"

namespace {
class ServerApp : public Poco::Util::ServerApplication {
    using base = Poco::Util::ServerApplication;

    std::shared_ptr<const vehlwn::ApplicationSettings> m_application_settings;
    std::shared_ptr<vehlwn::MotionDataWorker> m_motion_data_worker;

protected:
    void initialize(Poco::Util::Application& self) override
    {
        BOOST_LOG_FUNCTION();
        const int count = loadConfiguration();
        base::initialize(self);

        m_application_settings = std::make_shared<const vehlwn::ApplicationSettings>(
            vehlwn::read_settings());
        vehlwn::init_logging(m_application_settings->logging.log_level);
        BOOST_LOG_TRIVIAL(info) << "Loaded " << count << " configuration files";

        auto back_subtractor_factory
            = std::make_shared<vehlwn::BackgroundSubtractorFactory>(
                m_application_settings->background_subtractor);

        auto input_device
            = vehlwn::FfmpegInputDeviceFactory(*m_application_settings).create();

        auto out_filename_factory = std::make_shared<vehlwn::DateFolderFactory>();
        out_filename_factory->set_prefix(
            std::string(m_application_settings->output_files.prefix));
        out_filename_factory->set_extension(
            std::string(m_application_settings->output_files.extension));

        auto preprocess_image_factory
            = std::make_shared<vehlwn::PreprocessImageFactory>(
                m_application_settings->preprocess);
        m_motion_data_worker = std::make_shared<vehlwn::MotionDataWorker>(
            std::move(back_subtractor_factory),
            std::move(input_device),
            std::move(out_filename_factory),
            std::move(preprocess_image_factory));
    }

    int main(const std::vector<std::string>& /*args*/) override
    {
        BOOST_LOG_FUNCTION();

        m_motion_data_worker->start();

        auto srv = Poco::Net::HTTPServer(
            new vehlwn::AppRequestHandlerFactory(
                std::shared_ptr(m_motion_data_worker)),
            Poco::Net::ServerSocket(Poco::Net::SocketAddress("[::1]:8080")),
            new Poco::Net::HTTPServerParams);
        srv.start();

        BOOST_LOG_TRIVIAL(info)
            << "Server listening " << srv.socket().address().toString();
        waitForTerminationRequest();
        srv.stopAll(true);

        m_motion_data_worker->stop();
        BOOST_LOG_TRIVIAL(debug)
            << "motion_data_worker.use_count = " << m_motion_data_worker.use_count();
        return Poco::Util::Application::EXIT_OK;
    }
};
} // namespace

POCO_SERVER_MAIN(ServerApp)
