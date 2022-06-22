#include "ImencodeHandler.hpp"

#include "fmt/core.h"
#include "opencv2/imgcodecs.hpp"

namespace vehlwn::handlers::common {
ImencodeHandler::ImencodeHandler(Poco::Logger& logger)
    : m_logger{logger} {
}

void ImencodeHandler::send_encoded_image(
    const cv::Mat& image,
    Poco::Net::HTTPServerResponse& response) {
    const std::string format = ".jpg";
    try {
        std::vector<unsigned char> buf;
        const bool result = cv::imencode(format, image, buf);
        if(result) {
            poco_information(
                m_logger,
                fmt::format("Saved {} file, buf.size = {}", format, buf.size()));
            response.setContentType("image/jpeg");
            m_content_length_handler.send(buf, response);
        } else {
            const std::string error_msg = fmt::format("Can't save {} file.", format);
            poco_warning(m_logger, error_msg);
            response.setStatusAndReason(
                Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("text/plain");
            m_content_length_handler.send(error_msg, response);
        }
    } catch(const cv::Exception& ex) {
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

} // namespace vehlwn::handlers::common
