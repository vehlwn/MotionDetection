#include "MotionDataWorker.hpp"

#include <cstdlib>
#include <memory>

#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>

#include "CvMatRaiiAdapter.hpp"

namespace vehlwn {
MotionDataWorker::MotionDataWorker(
    std::shared_ptr<BackgroundSubtractorFactory>&& back_subtractor_factory,
    ffmpeg::InputDevice&& input_device,
    std::shared_ptr<FileNameFactory>&& out_filename_factory,
    std::shared_ptr<PreprocessImageFactory>&& preprocess_image_factory)
    : m_back_subtractor_factory{std::move(back_subtractor_factory)}
    , m_input_device(std::move(input_device))
    , m_out_filename_factory(std::move(out_filename_factory))
    , m_preprocess_image_factory(std::move(preprocess_image_factory))
    , m_motion_data{std::make_shared<SharedMutex<MotionData>>()}
    , m_stopped{false}
{
    BOOST_LOG_FUNCTION();
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
    const auto fname = m_out_filename_factory->generate();
    m_input_device.start_recording(fname.data());
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
    while(!m_stopped) {
        auto frame = m_input_device.get_video_frame();
        auto processed = preprocess_filter->apply(frame.clone());
        auto fgmask = back_subtractor->apply(std::move(processed));
        (*m_motion_data->lock())
            .set_frame(std::move(frame))
            .set_fgmask(std::move(fgmask));
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
