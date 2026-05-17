add_executable(example
    src/main.cpp
    src/App.cpp
)

target_link_libraries(example PRIVATE MAGE::mage)

target_compile_definitions(example PRIVATE
    EXAMPLE_TEXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/textures"
)