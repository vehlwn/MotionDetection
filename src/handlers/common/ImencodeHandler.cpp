#include "ImencodeHandler.hpp"

#include <string>

#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>
#include <opencv2/imgcodecs.hpp>

namespace vehlwn::handlers::common {
void ImencodeHandler::send_encoded_image(
    const cv::Mat& image,
    Poco::Net::HTTPServerResponse& response)
{
    BOOST_LOG_FUNCTION();
    const std::string format = ".jpg";
    try {
        std::vector<unsigned char> buf;
        const bool result = cv::imencode(format, image, buf);
        if(result) {
            BOOST_LOG_TRIVIAL(debug)
                << "Saved " << format << "file, buf.size =" << buf.size();
            response.setContentType("image/jpeg");
            m_content_length_handler.send(buf, response);
        } else {
            const auto error_msg = std::string("Can't save ") + format + " file.";
            BOOST_LOG_TRIVIAL(warning) << error_msg;
            response.setStatusAndReason(
                Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("text/plain");
            m_content_length_handler.send(error_msg, response);
        }
    } catch(const cv::Exception& ex) {
        const auto error_msg = std::string("Exception converting image to ") + format
            + " format: " + ex.what();
        BOOST_LOG_TRIVIAL(warning) << error_msg;
        response.setStatusAndReason(
            Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
        response.setContentType("text/plain");
        m_content_length_handler.send(error_msg, response);
    }
}

} // namespace vehlwn::handlers::common
