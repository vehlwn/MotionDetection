project(
  'MotionDetection',
  ['cpp'],
  default_options: ['cpp_std=c++17', 'default_library=static'],
  license: 'ISC',
  meson_version: '>=0.61'
)

subdir('meson')
add_project_arguments(HARDEN_CXX_OPTIONS, language: 'cpp')

prefix = get_option('prefix')
data_dir = prefix / get_option('datadir') / meson.project_name()
config_dir = prefix / get_option('sysconfdir') / meson.project_name()

subdir('src')

install_data(
  [
    'app.ini',
    'drogon.json',
  ],
  install_dir: config_dir)
install_subdir('front', install_dir: data_dir)
