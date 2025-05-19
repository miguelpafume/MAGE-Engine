cmake_minimum_required(VERSION 3.26)
project(mage_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

add_compile_definitions(SOURCE_DIR="${CMAKE_SOURCE_DIR}")

include(FetchContent)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
  SOURCE_DIR ${INCLUDE_DIR}/glm
)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
  SOURCE_DIR ${INCLUDE_DIR}/glfw
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glm glfw)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${SRC_DIR}/*.hpp ${SRC_DIR}/*.h)
file(GLOB_RECURSE SHADERS ${CMAKE_SOURCE_DIR}/shaders/*.*)
file(GLOB_RECURSE SCRIPTS ${CMAKE_SOURCE_DIR}/scripts/*.*)

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${HEADERS})
source_group("Shaders" FILES ${SHADERS})
source_group("Scripts" FILES ${SCRIPTS})

set_target_properties(glfw PROPERTIES FOLDER "Dependencies")
set_target_properties(update_mappings PROPERTIES FOLDER "Dependencies")
set_target_properties(glm PROPERTIES FOLDER "Dependencies")

add_executable(${PROJECT_NAME}
  ${SOURCES}
  ${HEADERS}
  ${SHADERS}
  ${SCRIPTS}
)

target_include_directories(${PROJECT_NAME}
PRIVATE
  ${SRC_DIR}
  ${INCLUDE_DIR}
  ${glm_SOURCE_DIR}
  ${glfw_SOURCE_DIR}/include
  ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
  glfw
  glm
  Vulkan::Vulkan
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
