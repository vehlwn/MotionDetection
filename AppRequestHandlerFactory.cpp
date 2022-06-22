#include "AppRequestHandlerFactory.h"

#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/URI.h"
#include "fmt/core.h"
#include "opencv2/imgcodecs.hpp"

#include <Poco/Logger.h>
#include <utility>
#include <vector>

namespace vehlwn {
namespace detail {
class ContentLengthHandler
{
public:
    void send(const std::string& msg, Poco::Net::HTTPServerResponse& response)
    {
        response.sendBuffer(reinterpret_cast<const void*>(msg.data()), msg.size());
    }

    void send(
        const std::vector<unsigned char>& msg,
        Poco::Net::HTTPServerResponse& response)
    {
        response.sendBuffer(reinterpret_cast<const void*>(msg.data()), msg.size());
    }
};

class OkHandler
{
public:
    void send(Poco::Net::HTTPServerResponse& response)
    {
        response.setContentType("text/plain");
        m_content_length_handler.send("ok", response);
    }

private:
    ContentLengthHandler m_content_length_handler;
};

class HealthyHandler : public Poco::Net::HTTPRequestHandler
{
public:
    virtual void handleRequest(
        Poco::Net::HTTPServerRequest& /*request*/,
        Poco::Net::HTTPServerResponse& response) override
    {
        m_ok_handler.send(response);
    }

private:
    OkHandler m_ok_handler;
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
        m_content_length_handler.send(msg, response);
    }

private:
    ContentLengthHandler m_content_length_handler;
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
                    fmt::format("Saved {} file, buf.size = {}", format, buf.size()));
                response.setContentType("image/jpeg");
                m_content_length_handler.send(buf, response);
            }
            else
            {
                const std::string error_msg =
                    fmt::format("Can't save {} file.", format);
                poco_warning(m_logger, error_msg);
                response.setStatusAndReason(
                    Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
                response.setContentType("text/plain");
                m_content_length_handler.send(error_msg, response);
            }
        }
        catch(const cv::Exception& ex)
        {
            const std::string error_msg = fmt::format(
                "Exception converting image to {} format: {}",
                format,
                ex.what());
            poco_warning(m_logger, error_msg);
            response.setStatusAndReason(
                Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("text/plain");
            m_content_length_handler.send(error_msg, response);
        }
    }

private:
    Poco::Logger& m_logger;
    ContentLengthHandler m_content_length_handler;
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
        Poco::Net::HTTPServerRequest& /*request*/,
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
        Poco::Net::HTTPServerRequest& /*request*/,
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

class IndexHandler : public Poco::Net::HTTPRequestHandler
{
public:
    virtual void handleRequest(
        Poco::Net::HTTPServerRequest& /*request*/,
        Poco::Net::HTTPServerResponse& response) override
    {
        response.setContentType("text/html");
        const std::string msg = R"(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>MotionDetection</title>
  </head>
  <body>
    <h1>ass</h1>
  </body>
</html>

)";
        m_content_length_handler.send(msg, response);
    }

private:
    ContentLengthHandler m_content_length_handler;
};
} // namespace detail

AppRequestHandlerFactory::AppRequestHandlerFactory(
    std::shared_ptr<vehlwn::MotionDataWorker> motion_data_worker,
    Poco::Logger& logger)
    : m_motion_data_worker{std::move(motion_data_worker)}
    , m_logger{logger}
{
    auto* logger_copy = &m_logger;
    auto motion_data_worker_copy = m_motion_data_worker;
    m_routes[{"GET", "/api/healthy"}] = [] { return new detail::HealthyHandler; };
    m_routes[{"GET", "/api/current_frame"}] = [=] {
        return new detail::CurrentFrameHandler{
            motion_data_worker_copy,
            *logger_copy};
    };
    m_routes[{"GET", "/api/motion_mask"}] = [=] {
        return new detail::MotionMaskHandler{motion_data_worker_copy, *logger_copy};
    };
    m_routes[{"GET", "/front/index.html"}] = [] { return new detail::IndexHandler; };
}

Poco::Net::HTTPRequestHandler* AppRequestHandlerFactory::createRequestHandler(
    const Poco::Net::HTTPServerRequest& request)
{
    poco_information(
        m_logger,
        fmt::format(
            "{} {} {}",
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
