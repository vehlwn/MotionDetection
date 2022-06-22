#include "IndexHandler.h"

namespace {
constexpr const char INDEX_HTML[] = R"(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>MotionDetection</title>
  <body>
    <div>
      <label for="fps">FPS:</label>
      <input type="number" id="fps" name="fps" min="1" max="100" value="10" />
      <input
        type="button"
        id="start_button"
        value="Start"
        style="margin: 25px"
      />
    </div>
    <div>
      <img id="current_frame" src="" alt="Current frame" />
      <img id="motion_mask" src="" alt="Motion mask" />
    </div>
  </body>
  <script>
    const fps_element = document.getElementById("fps");
    const start_button_element = document.getElementById("start_button");
    let fps = 0;
    let poll_image_interval;
    let stopped = true;
    window.onload = async function (_event) {
      try {
        const response = await get_data("/api/fps");
        const tmp_text = await response.text();
        fps = parseFloat(tmp_text);
        console.log(`fps = ${fps}`);
        fps_element.value = fps;
      } catch (e) {
        console.log(e.message);
      }
    };

    fps_element.oninput = function () {
      console.log(`fps input: ${fps_element.value}`);
      fps = parseFloat(fps_element.value);
      start_polling();
    };

    function start_polling() {
      stopped = false;
      start_button_element.value = "Stop";
      clearInterval(poll_image_interval);
      poll_image_interval = setInterval(poll_images, (1.0 / fps) * 1000.0);
    }

    function stop_polling() {
      stopped = true;
      start_button_element.value = "Start";
      clearInterval(poll_image_interval);
    }

    start_button_element.onclick = function () {
      if (stopped) {
        start_polling();
      } else {
        stop_polling();
      }
    };

    async function poll_image(id) {
      const response = await get_data(`/api/${id}`);
      const blob = await response.blob();
      URL.revokeObjectURL(document.getElementById(id).src);
      document.getElementById(id).src = URL.createObjectURL(blob);
    }

    async function poll_images() {
      try {
        await poll_image("current_frame");
        await poll_image("motion_mask");
      } catch (e) {
        console.log(e.message);
        stop_polling();
      }
    }

    async function get_data(url = "") {
      const response = await fetch(url, {
        method: "GET",
        headers: {
          pragma: "no-cache",
          "cache-control": "no-cache",
        },
      });
      if (!response.ok) {
        throw new Error(
          `HTTP error when fetching '${url}', status = ${response.status}`
        );
      }
      return response;
    }
  </script>
</html>
)";
}

namespace vehlwn::handlers {

void IndexHandler::handleRequest(
    Poco::Net::HTTPServerRequest& /*request*/,
    Poco::Net::HTTPServerResponse& response)
{
    response.setContentType("text/html");
    const std::string msg = INDEX_HTML;
    m_content_length_handler.send(msg, response);
}

} // namespace vehlwn::handlers
