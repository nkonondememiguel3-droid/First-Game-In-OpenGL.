cmake_minimum_required(VERSION 4.3)
project(first_opengl_game)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_EXTENSION OFF)

find_package(OpenGL REQUIRED)

if (CMAKE_HOST_WIN32)
    list(APPEND CMAKE_PREFIX_PATH "C:/softwares/SDL3/")
endif ()
find_package(SDL3 REQUIRED)

add_subdirectory(externals)

add_executable(first_opengl_game main.c
        src/utils.c
        src/include/utils.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL SDL3::SDL3 glad)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/include)
