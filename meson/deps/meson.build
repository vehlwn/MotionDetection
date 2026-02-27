opencv_dep = dependency('opencv', include_type: 'system')

drogon_dep = dependency('drogon', include_type: 'system')

boost_modules = ['log', 'thread', 'unit_test_framework']
boost_deps = []
boost_modules += 'headers'
foreach name : boost_modules
  boost_deps += dependency('boost-' + name, include_type: 'system')
endforeach

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
