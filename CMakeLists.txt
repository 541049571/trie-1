cmake_minimum_required (VERSION 2.8.11)

project(trie)

# Building
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(example)

# Tests
enable_testing()
add_test (NAME Units        COMMAND ./tests/bin/RootDiff)
add_test (NAME TrieTailDiff COMMAND ./tests/bin/tail_diff)
add_test (NAME TrieDict     COMMAND ./tests/bin/highload)
add_test (NAME Removing     COMMAND ./tests/bin/Removing)
