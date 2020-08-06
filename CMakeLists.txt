cmake_minimum_required(VERSION 3.5)

project(MotionDetection LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(
  Qt5 REQUIRED
  COMPONENTS Widgets
)

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(
    DOWNLOAD
    "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
  )
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(REQUIRES opencv/4.3.0@conan/stable OPTIONS opencv:ffmpeg=True opencv:parallel=openmp BASIC_SETUP BUILD missing)

if(WIN32)
  set(GUI_TYPE WIN32)
elseif(APPLE)
  set(GUI_TYPE MACOSX_BUNDLE)
endif()

add_executable(
  ${PROJECT_NAME}# ${GUI_TYPE}
  ApplicationSettings.cpp
  ApplicationSettings.h
  BufferedVideoReader.cpp
  BufferedVideoReader.h
  FixedThreadSafeQueue.h
  FrameConsumerThread.cpp
  FrameConsumerThread.h
  FrameProducerThread.cpp
  FrameProducerThread.h
  FrameWriterThread.cpp
  FrameWriterThread.h
  MainWindow.cpp
  MainWindow.h
  MainWindow.ui
  PixmapScene.cpp
  PixmapScene.h
  SettingsDialog.cpp
  SettingsDialog.h
  SettingsDialog.ui
  TimerWorker.h
  main.cpp
  utils.cpp
  utils.h
  PainterUtils.h
  PainterUtils.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets ${CONAN_LIBS})
