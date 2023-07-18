#include "MotionDataWorker.hpp"

#include <chrono>
#include <cstdlib>
#include <memory>

#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>

#include "CvMatRaiiAdapter.hpp"
#include "FfmpegInputDeviceFactory.hpp"

namespace vehlwn {
MotionDataWorker::MotionDataWorker(
    std::shared_ptr<const vehlwn::ApplicationSettings>&& settings)
    : m_back_subtractor_factory(
        std::make_shared<vehlwn::BackgroundSubtractorFactory>(
            settings->segmentation.background_subtractor))
    , m_input_device(vehlwn::FfmpegInputDeviceFactory(*settings).create())
    , m_preprocess_image_factory(
          std::make_shared<vehlwn::PreprocessImageFactory>(settings->preprocess))
    , m_settings(std::move(settings))
    , m_motion_data{std::make_shared<SharedMutex<MotionData>>()}
    , m_last_motion_point(std::chrono::system_clock::now())
    , m_stopped{false}
{
    BOOST_LOG_FUNCTION();
    m_out_filename_factory = [&] {
        auto ret = std::make_shared<vehlwn::DateFolderFactory>();
        ret->set_prefix(std::string(m_settings->output_files.prefix));
        ret->set_extension(std::string(m_settings->output_files.extension));
        return ret;
    }();
    BOOST_LOG_TRIVIAL(debug) << "constructor MotionDataWorker";
}

MotionDataWorker::~MotionDataWorker()
{
    BOOST_LOG_FUNCTION();
    BOOST_LOG_TRIVIAL(debug) << "destructor ~MotionDataWorker";
    if(!m_stopped) {
        stop();
    }
}

std::shared_ptr<const SharedMutex<MotionData>>
    MotionDataWorker::get_motion_data() const
{
    return m_motion_data;
}

void MotionDataWorker::start()
{
    m_stopped = false;
    auto back_subtractor = m_back_subtractor_factory->create();
    auto preprocess_filter = m_preprocess_image_factory->create();
    m_working_thread = std::thread(
        &MotionDataWorker::thread_func,
        this,
        std::move(back_subtractor),
        std::move(preprocess_filter));
}

void MotionDataWorker::thread_func(
    std::shared_ptr<IBackgroundSubtractor>&& back_subtractor,
    std::shared_ptr<IImageFilter>&& preprocess_filter)
{
    BOOST_LOG_FUNCTION();
    const auto& segmentation = m_settings->segmentation;
    auto output_path = std::string();
    while(!m_stopped) {
        auto frame = m_input_device.get_video_frame();
        auto processed = preprocess_filter->apply(frame.clone());
        auto fgmask = back_subtractor->apply(std::move(processed));
        (*m_motion_data->write())
            .set_frame(std::move(frame))
            .set_fgmask(std::move(fgmask));

        const auto current_moving_area = m_motion_data->read()->moving_area();
        const auto now = std::chrono::system_clock::now();
        if(current_moving_area >= segmentation.min_moving_area) {
            m_last_motion_point = now;
            if(!m_input_device.is_recording()) {
                output_path = m_out_filename_factory->generate();
                BOOST_LOG_TRIVIAL(info)
                    << "Motion detected. Opening file '" << output_path << "'";
                m_input_device.start_recording(output_path.data());
            }
        } else {
            if(m_input_device.is_recording()) {
                const auto duration
                    = std::chrono::duration<double>(now - m_last_motion_point)
                          .count();
                if(duration >= segmentation.delta_without_motion) {
                    BOOST_LOG_TRIVIAL(info)
                        << "End of motion. Closing file '" << output_path << "'";
                    m_input_device.stop_recording();
                }
            }
        }
    }
}

void MotionDataWorker::stop()
{
    BOOST_LOG_FUNCTION();
    BOOST_LOG_TRIVIAL(debug) << "Stopping...";
    m_stopped = true;
    if(m_working_thread.joinable()) {
        BOOST_LOG_TRIVIAL(debug) << "Joining working thread...";
        m_working_thread.join();
        BOOST_LOG_TRIVIAL(debug) << "Joined";
    }
}

double MotionDataWorker::get_fps() const
{
    return m_input_device.fps();
}

bool MotionDataWorker::is_recording() const
{
    return m_input_device.is_recording();
}
} // namespace vehlwn
