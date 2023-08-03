opencv_dep = dependency('opencv4', include_type: 'system')

drogon_dep = dependency('drogon', include_type: 'system', required: false)
if not drogon_dep.found() and get_option('use_subproj_drogon')
  cmake = import('cmake')
  opt_var = cmake.subproject_options()
  opt_var.add_cmake_defines(
    {
      'BUILD_CTL': false,
      'BUILD_EXAMPLES': false,
      'BUILD_SHARED_LIBS': false,
      'USE_SUBMODULE': true
    }
  )
  opt_var.set_install(false)
  drogon_subproj = cmake.subproject('drogon', options: opt_var)
  drogon_dep = [
    drogon_subproj.dependency('drogon', include_type: 'system'),
    drogon_subproj.dependency('trantor', include_type: 'system')
  ]
else
  error('Drogon not found! Please either install it to external location and set up PKG_CONFIG_PATH appropriately, or use conan, or use \'use_subproj_drogon\' option')
endif

boost_modules = ['log', 'thread', 'unit_test_framework']
boost_deps = []
if get_option('use_conan_boost')
  boost_modules += 'headers'
  foreach name : boost_modules
    boost_deps += dependency('boost-' + name, include_type: 'system')
  endforeach
else
  boost_deps = dependency('boost', modules: boost_modules, include_type: 'system')
endif

libav_deps = []
foreach name : [
  'libavcodec',
  'libavdevice',
  'libavformat',
  'libavutil',
  'libswresample',
  'libswscale',
  ]
  libav_deps += dependency(name, include_type: 'system')
endforeach
