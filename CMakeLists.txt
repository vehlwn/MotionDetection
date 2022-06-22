cmake_minimum_required(VERSION 3.5)

project(MotionDetection LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)
find_package(Poco REQUIRED COMPONENTS Net Foundation Util)
find_package(fmt REQUIRED)

set(SANITIZER "-g3 -O0 -fno-inline -fno-omit-frame-pointer -fsanitize=thread")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SANITIZER}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${SANITIZER}")

add_executable(
    ${PROJECT_NAME}
    src/ApplicationSettings.cpp
    src/ApplicationSettings.hpp
    src/AppRequestHandlerFactory.cpp
    src/AppRequestHandlerFactory.hpp
    src/filters/GaussianBlurFilter.cpp
    src/filters/GaussianBlurFilter.hpp
    src/filters/IdentityFilter.cpp
    src/filters/IdentityFilter.hpp
    src/filters/IImageFilter.hpp
    src/filters/ImageFilterChain.cpp
    src/filters/ImageFilterChain.hpp
    src/filters/MedianFilter.cpp
    src/filters/MedianFilter.hpp
    src/filters/NormalizedBoxFilter.cpp
    src/filters/NormalizedBoxFilter.hpp
    src/filters/ResizeFilter.cpp
    src/filters/ResizeFilter.hpp
    src/handlers/common/ContentLengthHandler.cpp
    src/handlers/common/ContentLengthHandler.hpp
    src/handlers/common/ImencodeHandler.cpp
    src/handlers/common/ImencodeHandler.hpp
    src/handlers/common/OkHandler.cpp
    src/handlers/common/OkHandler.hpp
    src/handlers/CurrentFrameHandler.cpp
    src/handlers/CurrentFrameHandler.hpp
    src/handlers/FpsHandler.cpp
    src/handlers/FpsHandler.hpp
    src/handlers/HealthyHandler.cpp
    src/handlers/HealthyHandler.hpp
    src/handlers/IndexHandler.cpp
    src/handlers/IndexHandler.hpp
    src/handlers/MotionMaskHandler.cpp
    src/handlers/MotionMaskHandler.hpp
    src/handlers/NotFoundHandler.cpp
    src/handlers/NotFoundHandler.hpp
    src/IVideoCapture.hpp
    src/main.cpp
    src/MotionData.hpp
    src/MotionDataWorker.cpp
    src/MotionDataWorker.hpp
    src/Mutex.hpp
    src/OpencvBackgroundSubtractorFactory.cpp
    src/OpencvBackgroundSubtractorFactory.hpp
    src/OpencvVideoCapture.cpp
    src/OpencvVideoCapture.hpp
    src/PreprocessImageFactory.cpp
    src/PreprocessImageFactory.hpp
    src/VideoCaptureFactory.cpp
    src/VideoCaptureFactory.hpp
)
target_link_libraries(
    ${PROJECT_NAME} PUBLIC ${OpenCV_LIBRARIES} ${Poco_LIBRARIES} fmt::fmt
)
