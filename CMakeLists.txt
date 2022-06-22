cmake_minimum_required(VERSION 3.5)

project(MotionDetection LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(
    DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
    TLS_VERIFY ON
  )
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(
  REQUIRES
  poco/1.11.1
  opencv/4.5.3
  BASIC_SETUP
  BUILD missing
)

add_executable(
  ${PROJECT_NAME}
  main.cpp
)
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
