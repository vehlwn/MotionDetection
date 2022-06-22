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
    ApplicationSettings.cpp
    ApplicationSettings.h
    AppRequestHandlerFactory.cpp
    AppRequestHandlerFactory.h
    handlers/common/ContentLengthHandler.cpp
    handlers/common/ContentLengthHandler.h
    handlers/common/ImencodeHandler.cpp
    handlers/common/ImencodeHandler.h
    handlers/common/OkHandler.cpp
    handlers/common/OkHandler.h
    handlers/CurrentFrameHandler.cpp
    handlers/CurrentFrameHandler.h
    handlers/FpsHandler.cpp
    handlers/FpsHandler.h
    handlers/HealthyHandler.cpp
    handlers/HealthyHandler.h
    handlers/IndexHandler.cpp
    handlers/IndexHandler.h
    handlers/MotionMaskHandler.cpp
    handlers/MotionMaskHandler.h
    handlers/NotFoundHandler.cpp
    handlers/NotFoundHandler.h
    IVideoCapture.h
    main.cpp
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
)
target_link_libraries(
    ${PROJECT_NAME} PUBLIC ${OpenCV_LIBRARIES} ${Poco_LIBRARIES} fmt::fmt
)
