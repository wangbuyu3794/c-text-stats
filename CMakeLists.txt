cmake_minimum_required(VERSION 3.16)

project(c_text_stats
    VERSION 1.0.0
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
