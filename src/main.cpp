#include <exception>
#include <iostream>
#include <memory>

#include <boost/algorithm/string/join.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <drogon/HttpAppFramework.h>
#include <drogon/plugins/AccessLogger.h>

#include "Api.hpp"
#include "ApplicationSettings.hpp"
#include "BackgroundSubtractorFactory.hpp"
#include "Config.hpp"
#include "DemuxerOptionsFactory.hpp"
#include "FfmpegInputDeviceFactory.hpp"
#include "FileNameFactory.hpp"
#include "MotionDataWorker.hpp"
#include "PreprocessImageFactory.hpp"
#include "init_logging.hpp"

int main()
try {
    BOOST_LOG_FUNCTION();
    const auto application_settings
        = std::make_shared<const vehlwn::ApplicationSettings>(
            vehlwn::read_settings());
    vehlwn::init_logging(application_settings->logging.log_level);

    auto back_subtractor_factory
        = std::make_shared<vehlwn::BackgroundSubtractorFactory>(
            application_settings->background_subtractor);

    auto input_device
        = vehlwn::FfmpegInputDeviceFactory(*application_settings).create();

    auto out_filename_factory = std::make_shared<vehlwn::DateFolderFactory>();
    out_filename_factory->set_prefix(
        std::string(application_settings->output_files.prefix));
    out_filename_factory->set_extension(
        std::string(application_settings->output_files.extension));

    auto preprocess_image_factory = std::make_shared<vehlwn::PreprocessImageFactory>(
        application_settings->preprocess);
    auto motion_data_worker = std::make_shared<vehlwn::MotionDataWorker>(
        std::move(back_subtractor_factory),
        std::move(input_device),
        std::move(out_filename_factory),
        std::move(preprocess_image_factory));
    motion_data_worker->start();

    drogon::app()
        .loadConfigFile(std::string(CONFIG_DIR) + "/drogon_config.json")
        .setDocumentRoot(std::string(DATA_DIR) + "/front")
        .registerController(
            std::make_shared<vehlwn::api::Controller>(std::move(motion_data_worker)))
        .registerBeginningAdvice([] {
            const auto gen_list = [] {
                auto ret = std::vector<std::string>();
                boost::transform(
                    drogon::app().getListeners(),
                    std::back_inserter(ret),
                    [](const auto& x) { return x.toIpPort(); });
                return boost::join(ret, ", ");
            };
            BOOST_LOG_TRIVIAL(info) << "Server listening " << gen_list();
        })
        .run();
    return 0;
} catch(const std::exception& ex) {
    BOOST_LOG_TRIVIAL(fatal) << ex.what();
}
