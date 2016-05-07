include_directories(../include)
add_executable(normal1 normal.c)
add_executable(highload highload.c)
add_executable(RootDiff RootDiff.c)
add_executable(tail_diff tail_diff.c)

target_link_libraries(highload LINK_PUBLIC trie)
target_link_libraries(normal1 LINK_PUBLIC trie)
target_link_libraries(RootDiff LINK_PUBLIC trie)
target_link_libraries(tail_diff LINK_PUBLIC trie)


set_target_properties(normal1 highload RootDiff tail_diff
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)
