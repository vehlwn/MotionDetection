#include "HardwareHelpers.hpp"

#include <boost/algorithm/string/join.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>

namespace vehlwn::ffmpeg::hw_helpers {
AVHWDeviceType find_hw_device_by_name(const char* const name)
{
    BOOST_LOG_FUNCTION();
    auto type = av_hwdevice_find_type_by_name(name);
    if(type == AV_HWDEVICE_TYPE_NONE) {
        const auto msg = std::string("Device type '") + name + "' is not supported";
        BOOST_LOG_TRIVIAL(error) << msg;

        auto avaliable_types = std::vector<std::string>();
        while((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE) {
            avaliable_types.emplace_back(av_hwdevice_get_type_name(type));
        }
        BOOST_LOG_TRIVIAL(error) << "Available device types: {"
                                 << boost::join(avaliable_types, ", ") << "}";
        throw std::runtime_error(msg);
    }
    return type;
}

AVPixelFormat find_hw_pix_fmt(
    const AVCodec* const c,
    const AVHWDeviceType type,
    const bool is_encoder)
{
    BOOST_LOG_FUNCTION();
    auto hw_pix_fmt = AV_PIX_FMT_NONE;
    const unsigned methods = is_encoder ? AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX
            | AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX
                                        : AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX;
    for(int i = 0;; i++) {
        const auto config = avcodec_get_hw_config(c, i);
        if(config == nullptr) {
            BOOST_LOG_TRIVIAL(error) << (is_encoder ? "Encoder " : "Decoder ")
                                     << c->name << " does not support device type "
                                     << av_hwdevice_get_type_name(type);
            throw std::runtime_error("Codec does not support wanted device type");
        }
        if(((static_cast<unsigned>(config->methods) & methods) != 0)
           && config->device_type == type) {
            hw_pix_fmt = config->pix_fmt;
            break;
        }
    }
    return hw_pix_fmt;
}
} // namespace vehlwn::ffmpeg::hw_helpers
