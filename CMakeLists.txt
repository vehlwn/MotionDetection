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

find_package(OpenCV REQUIRED)

if(WIN32)
  set(GUI_TYPE WIN32)
elseif(APPLE)
  set(GUI_TYPE MACOSX_BUNDLE)
endif()

add_executable(
  ${PROJECT_NAME} ${GUI_TYPE}
  ApplicationSettings.cpp
  ApplicationSettings.h
  BufferedVideoReader.cpp
  BufferedVideoReader.h
  FixedThreadSafeQueue.h
  FrameConsumerThread.cpp
  FrameConsumerThread.h
  FrameProducerThread.cpp
  FrameProducerThread.h
  MainWindow.cpp
  MainWindow.h
  MainWindow.ui
  PainterUtils.cpp
  PainterUtils.h
  PixmapScene.cpp
  PixmapScene.h
  PixmapTitleItem.cpp
  PixmapTitleItem.h
  SettingsDialog.cpp
  SettingsDialog.h
  SettingsDialog.ui
  TimerWorker.h
  main.cpp
  utils.cpp
  utils.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Widgets ${OpenCV_LIBS})
