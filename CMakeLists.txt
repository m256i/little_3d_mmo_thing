cmake_minimum_required(VERSION 3.26.1)

project(blue_mmo LANGUAGES C CXX)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

Include(FetchContent)
FetchContent_Declare(
    rlfw
    GIT_REPOSITORY https://github.com/Journeyman1337/rlfw
    GIT_TAG        0.1.1.0
)
set(RLFW_FETCH_DEPENDENCIES ON)
FetchContent_MakeAvailable(rlfw)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE rlfw::framework)
