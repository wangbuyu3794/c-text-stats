cmake_minimum_required(VERSION 3.16)

project(c_text_stats
    VERSION 1.1.0
    DESCRIPTION "A small command-line text statistics tool"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

function(enable_strict_warnings target_name)
    if(MINGW)
        target_compile_definitions(${target_name}
            PRIVATE
                __USE_MINGW_ANSI_STDIO=1
        )
    endif()

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()

add_library(text_stats_core
    src/text_stats.c
)

target_include_directories(text_stats_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_strict_warnings(text_stats_core)

add_executable(c-text-stats
    src/main.c
)

target_link_libraries(c-text-stats PRIVATE text_stats_core)
enable_strict_warnings(c-text-stats)

add_executable(test_text_stats
    tests/test_text_stats.c
)

target_link_libraries(test_text_stats PRIVATE text_stats_core)
enable_strict_warnings(test_text_stats)

enable_testing()
add_test(NAME text_stats_tests COMMAND test_text_stats)

add_test(
    NAME cli_multiple_files
    COMMAND c-text-stats
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
)

set_tests_properties(cli_multiple_files
    PROPERTIES
        PASS_REGULAR_EXPRESSION "total"
)

add_test(NAME cli_help COMMAND c-text-stats --help)

set_tests_properties(cli_help
    PROPERTIES
        PASS_REGULAR_EXPRESSION "Usage:"
)

add_test(
    NAME cli_continues_after_error
    COMMAND ${CMAKE_COMMAND}
        -DCLI=$<TARGET_FILE:c-text-stats>
        -DMISSING_FILE=${CMAKE_CURRENT_BINARY_DIR}/missing-input.txt
        -DVALID_FILE=${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cli_continues_after_error.cmake
)
