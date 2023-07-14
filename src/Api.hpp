#pragma once

#include <drogon/HttpController.h>

#include "MotionDataWorker.hpp"

namespace vehlwn::api {
class Controller : public drogon::HttpController<Controller, false> {
    std::shared_ptr<vehlwn::MotionDataWorker> m_motion_data_worker;

public:
    explicit Controller(
        std::shared_ptr<vehlwn::MotionDataWorker>&& motion_data_worker);

    METHOD_LIST_BEGIN
    ADD_METHOD_TO(Controller::healthy, "/api/healthy", drogon::Get);
    ADD_METHOD_TO(Controller::current_frame, "/api/current_frame", drogon::Get);
    ADD_METHOD_TO(Controller::motion_mask, "/api/motion_mask", drogon::Get);
    ADD_METHOD_TO(Controller::fps, "/api/fps", drogon::Get);
    ADD_METHOD_TO(Controller::moving_area, "/api/moving_area", drogon::Get);
    METHOD_LIST_END

private:
    using RespCb = std::function<void(const drogon::HttpResponsePtr&)>;
    static void healthy(const drogon::HttpRequestPtr& req, RespCb&& callback);
    void current_frame(const drogon::HttpRequestPtr& req, RespCb&& callback) const;
    void motion_mask(const drogon::HttpRequestPtr& req, RespCb&& callback) const;
    void fps(const drogon::HttpRequestPtr& req, RespCb&& callback) const;
    void moving_area(const drogon::HttpRequestPtr& req, RespCb&& callback) const;
};
} // namespace vehlwn::api
