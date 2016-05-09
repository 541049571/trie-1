include_directories(../include)
add_executable(visualizer visualizer.c)

target_link_libraries(visualizer LINK_PUBLIC trie)


set_target_properties(visualizer
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)
