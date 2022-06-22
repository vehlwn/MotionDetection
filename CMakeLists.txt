cmake_minimum_required(VERSION 3.5)

project(MotionDetection LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)
find_package(Poco REQUIRED COMPONENTS Net Foundation Util)
find_package(fmt REQUIRED)

add_executable(
  ${PROJECT_NAME}
  AppRequestHandlerFactory.cpp
  AppRequestHandlerFactory.h
  IVideoCapture.h
  MotionData.h
  MotionDataWorker.cpp
  MotionDataWorker.h
  Mutex.h
  OpencvBackgroundSubtractorFactory.cpp
  OpencvBackgroundSubtractorFactory.h
  OpencvVideoCapture.cpp
  OpencvVideoCapture.h
  VideoCaptureFactory.cpp
  VideoCaptureFactory.h
  main.cpp
)
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBRARIES} ${Poco_LIBRARIES} fmt::fmt)
