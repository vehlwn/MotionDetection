#include "IndexHandler.h"

namespace vehlwn::handlers {

void IndexHandler::handleRequest(
    Poco::Net::HTTPServerRequest& /*request*/,
    Poco::Net::HTTPServerResponse& response)
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

} // namespace vehlwn::handlers
