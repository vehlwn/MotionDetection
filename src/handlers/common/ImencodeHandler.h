#pragma once

#include "ContentLengthHandler.h"
#include "Poco/Logger.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "opencv2/core/mat.hpp"

#include <string>
#include <vector>

namespace vehlwn::handlers::common {
class ImencodeHandler
{
public:
    ImencodeHandler(Poco::Logger& logger);
    void send_encoded_image(
        const cv::Mat& image,
        Poco::Net::HTTPServerResponse& response);

private:
    Poco::Logger& m_logger;
    ContentLengthHandler m_content_length_handler;
};
} // namespace vehlwn::handlers::common
