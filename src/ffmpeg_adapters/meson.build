ffmpeg_adapters = library(
  'ffmpeg_adapters',
  [
    'detail/AvError.hpp',
    'detail/AvFrameAdapters.hpp',
    'detail/AvPacketAdapters.hpp',
    'detail/AVRationalOutput.hpp',
    'detail/BaseAvCodecContextProperties.hpp',
    'detail/HardwareHelpers.cpp',
    'detail/HardwareHelpers.hpp',
    'detail/OutputFile.cpp',
    'detail/OutputFile.hpp',
    'detail/ScopedAvAudioFifo.hpp',
    'detail/ScopedAvFormatInput.hpp',
    'detail/ScopedAvFormatOutput.hpp',
    'detail/ScopedAvSAmplesBuffer.hpp',
    'detail/ScopedDecoderContext.hpp',
    'detail/ScopedEncoderContext.hpp',
    'detail/SwrResampler.hpp',
    'detail/SwsPixelConverter.hpp',
    'InputDevice.cpp',
    'InputDevice.hpp',
    'ScopedAvDictionary.hpp',
    ],
  dependencies: [libav_deps, opencv_dep, boost_deps]
)

ffmpeg_adapters_dep = declare_dependency(link_with: ffmpeg_adapters)
