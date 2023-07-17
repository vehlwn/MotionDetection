project(
  'MotionDetection',
  ['cpp'],
  default_options: ['cpp_std=c++17'],
  license: 'ISC',
  meson_version: '>=0.61'
)

subdir('meson')
add_global_arguments(HARDEN_CXX_OPTIONS, language: 'cpp')

prefix = get_option('prefix')
data_dir = prefix / get_option('datadir') / meson.project_name()
config_dir = prefix / get_option('sysconfdir') / meson.project_name()

ini_proj = subproject('ini', default_options: ['default_library=static'])
ini_dep = ini_proj.get_variable('ini_dep')

subdir('src')

install_data(
  [
    'app.json',
    'drogon.json',
  ],
  install_dir: config_dir)
install_subdir('front', install_dir: data_dir)
