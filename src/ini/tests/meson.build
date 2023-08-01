boost_modules = ['unit_test_framework']
boost_deps = []
if get_option('use_conan_boost')
  foreach name : boost_modules
    boost_deps += dependency('boost-' + name, include_type: 'system')
  endforeach
else
  boost_deps = dependency('boost', modules: boost_modules, include_type: 'system')
endif

test('parser',
  executable(
    'parser',
    ['parser.cpp'],
    dependencies: [boost_deps, ini_dep],
    include_directories: src_dir
  )
)
