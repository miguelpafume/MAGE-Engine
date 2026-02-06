cmake_minimum_required(VERSION 3.26.0)
project(mage_engine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
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
message(STATUS "Vulkan include dir: $ENV{VULKAN_SDK}")

file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${SRC_DIR}/*.hpp ${SRC_DIR}/*.h)
file(GLOB_RECURSE SHADERS ${CMAKE_SOURCE_DIR}/shaders/*.slang)
file(GLOB_RECURSE SCRIPTS ${CMAKE_SOURCE_DIR}/scripts/*.*)

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${HEADERS})
source_group("Shaders" FILES ${SHADERS})
source_group("Scripts" FILES ${SCRIPTS})

set_target_properties(glfw PROPERTIES FOLDER "Dependencies")
set_target_properties(update_mappings PROPERTIES FOLDER "Dependencies")
set_target_properties(glm PROPERTIES FOLDER "Dependencies")

# Recompile shaders
find_program(SLANG_EXECUTABLE slangc HINTS ${Vulkan_BIN_DIR})

set(SPIRV_BINARY_FILES "")

foreach(SHADER_SOURCE ${SHADERS})
  set(SHADER_OUTPUT "${SHADERS}.spv")

  add_custom_command(
    OUTPUT ${SHADER_OUTPUT}
    COMMAND ${SLANG_EXECUTABLE} ${SHADER_SOURCE} -target spiriv -o ${SHADER_OUTPUT}
    DEPENDS ${SHADER_SOURCE}
    COMMENT "Compiling shader: ${SHADER_SOURCE}"
  )

  list(APPEND SPIRV_BINARY_FILES ${SHADER_OUTPUT})
endforeach()

add_executable(
  ${PROJECT_NAME}
  ${SOURCES}
  ${HEADERS}
  ${SHADERS}
  ${SCRIPTS}
)

target_include_directories(
  ${PROJECT_NAME} PRIVATE
  ${SRC_DIR}
  ${INCLUDE_DIR}
  ${glm_SOURCE_DIR}
  ${glfw_SOURCE_DIR}/include
  ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  glfw
  glm
  Vulkan::Vulkan
)

#Linux
if (UNIX AND NOT APPLE)
  find_package(X11 REQUIRED)

  target_link_libraries(${PROJECT_NAME} PRIVATE
    ${X11_LIBRARIES}
    Xrandr
    Xxf86vm
    Xi
    pthread
    dl
  )
endif()

# Windows (won't do anything on linux)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})