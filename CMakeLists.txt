cmake_minimum_required(VERSION 3.26)
project(mage_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

add_compile_definitions(SOURCE_DIR="${CMAKE_SOURCE_DIR}")

find_package(Vulkan REQUIRED)
add_subdirectory(${INCLUDE_DIR}/glfw)

file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${SRC_DIR}/*.hpp ${SRC_DIR}/*.h)

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${SRC_DIR}
    ${INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glfw
    Vulkan::Vulkan
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})