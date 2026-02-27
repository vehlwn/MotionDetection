#include <exception>
#include <iostream>
#include <memory>

#include <boost/algorithm/string/join.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <drogon/drogon.h>


#include "Api.hpp"
#include "ApplicationSettings.hpp"
#include "Config.hpp"
#include "MotionDataWorker.hpp"
#include "init_logging.hpp"

int main()
try {
    BOOST_LOG_FUNCTION();
    auto application_settings = std::make_shared<const vehlwn::ApplicationSettings>(
        vehlwn::read_settings());
    vehlwn::init_logging(application_settings->logging);

    auto motion_data_worker = std::make_shared<vehlwn::MotionDataWorker>(
        std::move(application_settings));
    motion_data_worker->start();

    drogon::app()
        .loadConfigFile(std::string(CONFIG_DIR) + "/drogon.json")
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
