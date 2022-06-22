#include "AppRequestHandlerFactory.h"

#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/URI.h"
#include "opencv2/imgcodecs.hpp"

#include <Poco/Logger.h>
#include <utility>
#include <vector>

namespace vehlwn {
namespace detail {
class LivenessHandler : public Poco::Net::HTTPRequestHandler
{
public:
    virtual void handleRequest(
        Poco::Net::HTTPServerRequest& /*request*/,
        Poco::Net::HTTPServerResponse& response) override
    {
        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
        response.setContentType("text/plain");
        const std::string msg = "ok";
        response.sendBuffer(msg.data(), msg.size());
    }
};

class NotFoundHandler : public Poco::Net::HTTPRequestHandler
{
public:
    virtual void handleRequest(
        Poco::Net::HTTPServerRequest& /*request*/,
        Poco::Net::HTTPServerResponse& response) override
    {
        response.setStatusAndReason(
            Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        response.setContentType("text/plain");
        const std::string msg = "Path not found";
        response.sendBuffer(msg.data(), msg.size());
    }
};

class ImencodeHandler
{
public:
    ImencodeHandler(Poco::Logger& logger)
        : m_logger{logger}
    {
    }

    void send_encoded_image(
        const cv::Mat& image,
        Poco::Net::HTTPServerResponse& response)
    {
        const std::string format = ".jpg";
        try
        {
            std::vector<unsigned char> buf;
            const bool result = cv::imencode(format, image, buf);
            if(result)
            {
                poco_information(
                    m_logger,
                    Poco::format(
                        "Saved %s file, buf.size = %z",
                        format,
                        buf.size()));
                response.setContentType("image/jpeg");
                response.sendBuffer(buf.data(), buf.size());
            }
            else
            {
                const std::string error_msg =
                    Poco::format("Can't save %s file.", format);
                poco_warning(m_logger, error_msg);
                response.setStatus(
                    Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
                response.setContentType("text/plain");
                response.sendBuffer(error_msg.data(), error_msg.size());
            }
        }
        catch(const cv::Exception& ex)
        {
            const std::string error_msg = Poco::format(
                "Exception converting image to %s format: %s",
                format,
                ex.what());
            poco_warning(m_logger, error_msg);
            response.setStatus(
                Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("text/plain");
            response.sendBuffer(error_msg.data(), error_msg.size());
        }
    }

private:
    Poco::Logger& m_logger;
};

class CurrentFrameHandler : public Poco::Net::HTTPRequestHandler
{
public:
    CurrentFrameHandler(
        std::shared_ptr<const vehlwn::MotionDataWorker> motion_data_worker,
        Poco::Logger& logger)
        : m_imencode_handler{logger}
        , m_motion_data_worker{std::move(motion_data_worker)}
    {
    }

    virtual void handleRequest(
        Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override
    {
        const cv::Mat frame =
            m_motion_data_worker->get_motion_data()->lock()->frame();
        m_imencode_handler.send_encoded_image(frame, response);
    }

private:
    ImencodeHandler m_imencode_handler;
    std::shared_ptr<const vehlwn::MotionDataWorker> m_motion_data_worker;
};

class MotionMaskHandler : public Poco::Net::HTTPRequestHandler
{
public:
    MotionMaskHandler(
        std::shared_ptr<const vehlwn::MotionDataWorker> motion_data_worker,
        Poco::Logger& logger)
        : m_imencode_handler{logger}
        , m_motion_data_worker{std::move(motion_data_worker)}
    {
    }

    virtual void handleRequest(
        Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) override
    {
        const cv::Mat frame =
            m_motion_data_worker->get_motion_data()->lock()->fgmask();
        m_imencode_handler.send_encoded_image(frame, response);
    }

private:
    ImencodeHandler m_imencode_handler;
    std::shared_ptr<const vehlwn::MotionDataWorker> m_motion_data_worker;
};

} // namespace detail

AppRequestHandlerFactory::AppRequestHandlerFactory(
    std::shared_ptr<const vehlwn::MotionDataWorker> motion_data_worker,
    Poco::Logger& logger)
    : m_motion_data_worker{std::move(motion_data_worker)}
    , m_logger{logger}
{
    auto* logger_copy = &m_logger;
    auto motion_data_worker_copy = m_motion_data_worker;
    m_routes[{"GET", "/healthy"}] = [] { return new detail::LivenessHandler; };
    m_routes[{"GET", "/current_frame"}] = [=] {
        return new detail::CurrentFrameHandler{
            motion_data_worker_copy,
            *logger_copy};
    };
    m_routes[{"GET", "/motion_mask"}] = [=] {
        return new detail::MotionMaskHandler{motion_data_worker_copy, *logger_copy};
    };
}

Poco::Net::HTTPRequestHandler* AppRequestHandlerFactory::createRequestHandler(
    const Poco::Net::HTTPServerRequest& request)
{
    poco_information(
        m_logger,
        Poco::format(
            "%s %s %s",
            request.clientAddress().toString(),
            request.getMethod(),
            request.getURI()));
    const Poco::URI url{request.getURI()};
    if(const auto it = m_routes.find(std::tie(request.getMethod(), url.getPath()));
       it != m_routes.end())
        return it->second();
    else
        return new detail::NotFoundHandler;
}
} // namespace vehlwn
