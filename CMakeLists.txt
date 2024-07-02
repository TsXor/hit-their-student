cmake_minimum_required(VERSION 3.15)

project(StudentScoreManager)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROGRAM_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE PROGRAM_SOURCES ${PROGRAM_SOURCE_ROOT}/*.cpp)
add_executable(${PROJECT_NAME} ${PROGRAM_SOURCES})
if(MINGW)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc)
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${PROGRAM_SOURCE_ROOT})
