if get_option('use_conan_boost')
  boost_deps = dependency('boost-headers', include_type: 'system')
else
  boost_deps = dependency('boost', include_type: 'system')
endif

lib_ini = library(
  'ini',
  [
    'Ini.hpp',
    'Parser.cpp',
    'Parser.hpp',
  ],
  dependencies: boost_deps
)
ini_dep = declare_dependency(
  link_with: lib_ini
)
