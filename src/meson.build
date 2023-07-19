opencv_dep = dependency('OpenCV', include_type: 'system')

drogon_dep = dependency('drogon', include_type: 'system')

boost_modules = ['chrono', 'log']
boost_deps = []
foreach name : boost_modules
  boost_deps += dependency('boost-' + name, include_type: 'system')
endforeach

subdir('ini')
subdir('ffmpeg_adapters')

conf_data = configuration_data()
conf_data.set('CONFIG_DIR', '"@0@"'.format(config_dir))
conf_data.set('DATA_DIR', '"@0@"'.format(data_dir))
configure_file(
  input: 'Config.hpp.in',
  output: 'Config.hpp',
  configuration: conf_data
)

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
    'FfmpegInputDeviceFactory.hpp',
    'FileNameFactory.hpp',
    'filters/ConvertToGrayFilter.cpp',
    'filters/ConvertToGrayFilter.hpp',
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
  dependencies: [drogon_dep, opencv_dep, boost_deps, ini_dep, ffmpeg_adapters_dep],
  install: true,
)
