boost_deps = dependency('boost-headers', include_type: 'system')
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
