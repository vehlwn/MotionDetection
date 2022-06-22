cmake_minimum_required(VERSION 3.5)

project(MotionDetection LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror=pedantic -pedantic-errors \
    -Werror=return-type -Werror=return-local-addr -Werror=conversion \
    -Werror=sign-conversion")
# set(SANITIZER "-g3 -O0 -fno-inline -fno-omit-frame-pointer -fsanitize=thread")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SANITIZER}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${SANITIZER}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/bin
    CACHE PATH "Single Directory for all"
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/bin
    CACHE PATH "Single Directory for all"
)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/lib
    CACHE PATH "Single Directory for all"
)

include_directories(src)
add_subdirectory(src)
