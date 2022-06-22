cmake_minimum_required(VERSION 3.5)

project(MotionDetection LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS
    "-Wall -Wextra -Werror=pedantic -pedantic-errors \
    -Werror=return-type -Werror=conversion \
    -Werror=sign-conversion -Werror=switch -Werror=implicit-fallthrough"
)
set(WERROR_RETURN_LOCAL_ADDR)
set(WERROR_DELETE_NONABSTRACT_NONVIRTUAL_DTOR)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(WERROR_RETURN_LOCAL_ADDR "-Werror=return-stack-address")
    set(WERROR_DELETE_NONABSTRACT_NONVIRTUAL_DTOR
        "-Werror=delete-non-abstract-non-virtual-dtor"
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(WERROR_RETURN_LOCAL_ADDR "-Werror=return-local-addr")
    set(WERROR_DELETE_NONABSTRACT_NONVIRTUAL_DTOR "")
endif()
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} ${WERROR_RETURN_LOCAL_ADDR} ${WERROR_DELETE_NONABSTRACT_NONVIRTUAL_DTOR}"
)

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
