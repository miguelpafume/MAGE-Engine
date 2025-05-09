cmake_minimum_required(VERSION 3.20)
project(MAGE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/incldue")

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)

add_subdirectory(${INCLUDE_DIR}/glfw)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${SRC_DIR}
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/glm
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glfw
    Vulkan::Vulkan
)

