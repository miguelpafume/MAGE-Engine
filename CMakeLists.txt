cmake_minimum_required(VERSION 3.26.0)
project(mage VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MAGE_BUILD_EXAMPLES "Build MAGE example apps" ${PROJECT_IS_TOP_LEVEL})

set(MAGE_DIR      "${CMAKE_CURRENT_SOURCE_DIR}")
set(MAGE_SRC_DIR  "${MAGE_DIR}/src")
set(MAGE_INCLUDE_DIR  "${MAGE_DIR}/include")

# Third-party depencencies
include(FetchContent)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
  SOURCE_DIR ${MAGE_INCLUDE_DIR}/glm
)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
  SOURCE_DIR ${MAGE_INCLUDE_DIR}/glfw
)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.92.7
  SOURCE_DIR ${MAGE_INCLUDE_DIR}/imgui
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glm glfw imgui)

find_package(Vulkan REQUIRED)
message(STATUS "Vulkan include dir: $ENV{VULKAN_SDK}")

# ImGUI
# Since it doesn't have a CMakeLists.txt, it is manually added the source files to create a target for it
add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp

  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw Vulkan::Vulkan)

# MAGE library
file(GLOB MAGE_SOURCES CONFIGURE_DEPENDS ${MAGE_SRC_DIR}/*.cpp)
file(GLOB MAGE_HEADERS CONFIGURE_DEPENDS ${MAGE_SRC_DIR}/*.hpp ${MAGE_SRC_DIR}/*.h)

add_library(mage STATIC ${MAGE_SOURCES} ${MAGE_HEADERS})
add_library(MAGE::mage ALIAS mage)

# Build shaders
if (MAGE_BUILD_EXAMPLES)
  add_subdirectory(example)
endif()

find_program(SLANGC slangc REQUIRED)

file(GLOB MAGE_VERT_SHADERS     CONFIGURE_DEPENDS ${MAGE_DIR}/shaders/*.vert.slang)
file(GLOB MAGE_FRAG_SHADERS     CONFIGURE_DEPENDS ${MAGE_DIR}/shaders/*.frag.slang)
file(GLOB MAGE_SHADER_INCLUDES  CONFIGURE_DEPENDS ${MAGE_DIR}/shaders/*.slang)

set(MAGE_SPV_OUTPUTS "")

foreach(shader IN LISTS MAGE_VERT_SHADERS)
  add_custom_command(
    OUTPUT ${shader}.spv
    COMMAND ${SLANGC} ${shader} -stage vertex -target spirv -o ${shader}.spv
    DEPENDS ${shader} ${MAGE_SHADER_INCLUDES}
    VERBATIM
  )
  list(APPEND MAGE_SPV_OUTPUTS ${shader}.spv)
endforeach()

foreach(shader IN LISTS MAGE_FRAG_SHADERS)
  add_custom_command(
    OUTPUT ${shader}.spv
    COMMAND ${SLANGC} ${shader} -stage fragment -target spirv -o ${shader}.spv
    DEPENDS ${shader} ${MAGE_SHADER_INCLUDES}
    VERBATIM
  )
  list(APPEND MAGE_SPV_OUTPUTS ${shader}.spv)
endforeach()

add_custom_target(mage_shaders ALL DEPENDS ${MAGE_SPV_OUTPUTS})
add_dependencies(mage mage_shaders)

target_compile_features(mage PUBLIC cxx_std_20)

target_include_directories(mage PUBLIC
  $<BUILD_INTERFACE:${MAGE_SRC_DIR}>
  $<BUILD_INTERFACE:${MAGE_INCLUDE_DIR}>
)

target_compile_definitions(mage PRIVATE
  MAGE_SHADER_DIR="${MAGE_DIR}/shaders")

target_link_libraries(mage
  PUBLIC glfw glm Vulkan::Vulkan imgui
)

if (UNIX AND NOT APPLE)                                                                                           
    find_package(X11 REQUIRED)
    target_link_libraries(mage PUBLIC ${X11_LIBRARIES} Xrandr Xxf86vm Xi pthread dl)                              
endif()