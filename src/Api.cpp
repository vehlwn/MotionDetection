#include "Api.hpp"

#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <opencv2/imgcodecs.hpp>
#include <sstream>

namespace vehlwn::api {

namespace {
drogon::HttpResponsePtr create_encoded_image_resp(const cv::Mat& image)
{
    BOOST_LOG_FUNCTION();
    constexpr auto format = ".jpg";
    auto ret = drogon::HttpResponsePtr();
    try {
        auto buf = std::vector<unsigned char>();
        const bool result = cv::imencode(format, image, buf);
        if(result) {
            BOOST_LOG_TRIVIAL(debug)
                << "Saved " << format << "file, buf.size =" << buf.size();
            ret = drogon::HttpResponse::newFileResponse(
                buf.data(),
                buf.size(),
                "",
                drogon::CT_IMAGE_JPG);
        } else {
            std::ostringstream os;
            os << "Can't save " << format << " file.";
            auto error_msg = os.str();
            BOOST_LOG_TRIVIAL(warning) << error_msg;
            ret = drogon::HttpResponse::newHttpResponse();
            ret->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
            ret->setContentTypeCode(drogon::CT_TEXT_PLAIN);
            ret->setBody(std::move(error_msg));
        }
    } catch(const cv::Exception& ex) {
        std::ostringstream os;
        os << "Exception converting image to " << format << " format: " << ex.what();
        auto error_msg = os.str();
        BOOST_LOG_TRIVIAL(warning) << error_msg;
        ret = drogon::HttpResponse::newHttpResponse();
        ret->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
        ret->setContentTypeCode(drogon::CT_TEXT_PLAIN);
        ret->setBody(std::move(error_msg));
    }
    return ret;
}
} // namespace

Controller::Controller(
    std::shared_ptr<vehlwn::MotionDataWorker>&& motion_data_worker)
    : m_motion_data_worker(std::move(motion_data_worker))
{}

void Controller::healthy(const drogon::HttpRequestPtr& /*req*/, RespCb&& callback)
{
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
    resp->setBody("ok");
    callback(resp);
}

void Controller::current_frame(
    const drogon::HttpRequestPtr& /*req*/,
    RespCb&& callback) const
{
    cv::Mat frame;
    {
        const auto lock = m_motion_data_worker->get_motion_data()->lock();
        frame = lock->frame().get().clone();
    }
    callback(create_encoded_image_resp(frame));
}

void Controller::motion_mask(
    const drogon::HttpRequestPtr& /*req*/,
    RespCb&& callback) const
{
    cv::Mat mask;
    {
        const auto lock = m_motion_data_worker->get_motion_data()->lock();
        mask = lock->fgmask().get().clone();
    }
    callback(create_encoded_image_resp(mask));
}

void Controller::fps(const drogon::HttpRequestPtr& /*req*/, RespCb&& callback) const
{
    const double fps = m_motion_data_worker->get_fps();
    auto msg = std::to_string(fps);
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
    resp->setBody(std::move(msg));
    callback(resp);
}

void Controller::moving_area(
    const drogon::HttpRequestPtr& /*req*/,
    RespCb&& callback) const
{
    int ret = 0;
    {
        const auto lock = m_motion_data_worker->get_motion_data()->lock();
        ret = lock->moving_area();
    }
    auto msg = std::to_string(ret);
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
    resp->setBody(std::move(msg));
    callback(resp);
}

} // namespace vehlwn::api
