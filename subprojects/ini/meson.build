project(
  'ini',
  ['cpp'],
  default_options: ['cpp_std=c++17'],
  license: 'ISC',
  meson_version: '>=0.60'
)

src_dir = include_directories('src')
subdir('src')
if get_option('build_testing')
  subdir('tests')
endif
