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
    src/ApplicationSettings.h
    src/AppRequestHandlerFactory.cpp
    src/AppRequestHandlerFactory.h
    src/GaussianBlurFilter.cpp
    src/GaussianBlurFilter.h
    src/handlers/common/ContentLengthHandler.cpp
    src/handlers/common/ContentLengthHandler.h
    src/handlers/common/ImencodeHandler.cpp
    src/handlers/common/ImencodeHandler.h
    src/handlers/common/OkHandler.cpp
    src/handlers/common/OkHandler.h
    src/handlers/CurrentFrameHandler.cpp
    src/handlers/CurrentFrameHandler.h
    src/handlers/FpsHandler.cpp
    src/handlers/FpsHandler.h
    src/handlers/HealthyHandler.cpp
    src/handlers/HealthyHandler.h
    src/handlers/IndexHandler.cpp
    src/handlers/IndexHandler.h
    src/handlers/MotionMaskHandler.cpp
    src/handlers/MotionMaskHandler.h
    src/handlers/NotFoundHandler.cpp
    src/handlers/NotFoundHandler.h
    src/IdentityFilter.cpp
    src/IdentityFilter.h
    src/IImageFilter.h
    src/ImageFilterChain.cpp
    src/ImageFilterChain.h
    src/IVideoCapture.h
    src/main.cpp
    src/MedianFilter.cpp
    src/MedianFilter.h
    src/MotionData.h
    src/MotionDataWorker.cpp
    src/MotionDataWorker.h
    src/Mutex.h
    src/NormalizedBoxFilter.cpp
    src/NormalizedBoxFilter.h
    src/OpencvBackgroundSubtractorFactory.cpp
    src/OpencvBackgroundSubtractorFactory.h
    src/OpencvVideoCapture.cpp
    src/OpencvVideoCapture.h
    src/PreprocessImageFactory.cpp
    src/PreprocessImageFactory.h
    src/ResizeFilter.cpp
    src/ResizeFilter.h
    src/VideoCaptureFactory.cpp
    src/VideoCaptureFactory.h
)
target_link_libraries(
    ${PROJECT_NAME} PUBLIC ${OpenCV_LIBRARIES} ${Poco_LIBRARIES} fmt::fmt
)
