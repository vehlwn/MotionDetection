boost_deps = dependency('boost-unit_test_framework', include_type: 'system')
test('parser',
  executable(
    'parser',
    ['parser.cpp'],
    dependencies: [boost_deps, ini_dep],
    include_directories: src_dir
  )
)
