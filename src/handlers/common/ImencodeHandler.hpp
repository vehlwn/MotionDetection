#pragma once

#include <string>
#include <vector>

#include <Poco/Net/HTTPServerResponse.h>
#include <opencv2/core/mat.hpp>

#include "ContentLengthHandler.hpp"

namespace vehlwn::handlers::common {
class ImencodeHandler {
public:
    void send_encoded_image(
        const cv::Mat& image,
        Poco::Net::HTTPServerResponse& response);

private:
    ContentLengthHandler m_content_length_handler;
};
} // namespace vehlwn::handlers::common
