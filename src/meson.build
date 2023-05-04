opencv_dep = dependency('OpenCV', include_type: 'system')

drogon_dep = dependency('drogon', include_type: 'system')

boost_modules = ['chrono', 'log', 'thread', 'json']
boost_deps = dependency('boost', modules: boost_modules, include_type: 'system')

libav_deps = []
foreach name : [
    'libavcodec',
    'libavdevice',
    'libavformat',
    'libavutil',
    'libswresample',
    'libswscale',
  ]
  dep = dependency(name, include_type: 'system')
  libav_deps += dep
endforeach

executable(
  meson.project_name(),
  [
    'Api.cpp',
    'Api.hpp',
    'ApplicationSettings.cpp',
    'ApplicationSettings.hpp',
    'BackgroundSubtractorFactory.cpp',
    'BackgroundSubtractorFactory.hpp',
    'CvMatRaiiAdapter.hpp',
    'DemuxerOptionsFactory.hpp',
    'ErrorWithContext.hpp',
    'ffmpeg/detail/AvError.hpp',
    'ffmpeg/detail/AvFrameAdapters.hpp',
    'ffmpeg/detail/AvPacketAdapters.hpp',
    'ffmpeg/detail/AVRationalOutput.hpp',
    'ffmpeg/detail/BaseAvCodecContextProperties.hpp',
    'ffmpeg/detail/OutputFile.cpp',
    'ffmpeg/detail/OutputFile.hpp',
    'ffmpeg/detail/ScopedAvAudioFifo.hpp',
    'ffmpeg/detail/ScopedAvFormatInput.hpp',
    'ffmpeg/detail/ScopedAvFormatOutput.hpp',
    'ffmpeg/detail/ScopedAvSAmplesBuffer.hpp',
    'ffmpeg/detail/ScopedDecoderContext.hpp',
    'ffmpeg/detail/ScopedEncoderContext.hpp',
    'ffmpeg/detail/SwrResampler.hpp',
    'ffmpeg/detail/SwsPixelConverter.hpp',
    'ffmpeg/InputDevice.cpp',
    'ffmpeg/InputDevice.hpp',
    'ffmpeg/ScopedAvDictionary.hpp',
    'FfmpegInputDeviceFactory.hpp',
    'FileNameFactory.hpp',
    'filters/GaussianBlurFilter.cpp',
    'filters/GaussianBlurFilter.hpp',
    'filters/IdentityFilter.cpp',
    'filters/IdentityFilter.hpp',
    'filters/IImageFilter.hpp',
    'filters/ImageFilterChain.cpp',
    'filters/ImageFilterChain.hpp',
    'filters/MedianFilter.cpp',
    'filters/MedianFilter.hpp',
    'filters/NormalizedBoxFilter.cpp',
    'filters/NormalizedBoxFilter.hpp',
    'filters/ResizeFilter.cpp',
    'filters/ResizeFilter.hpp',
    'IBackgroundSubtractor.hpp',
    'init_logging.cpp',
    'init_logging.hpp',
    'main.cpp',
    'MotionData.cpp',
    'MotionData.hpp',
    'MotionDataWorker.cpp',
    'MotionDataWorker.hpp',
    'PreprocessImageFactory.cpp',
    'PreprocessImageFactory.hpp',
    'SharedMutex.hpp',
  ],
  dependencies: [drogon_dep, opencv_dep, boost_deps, libav_deps],
)
