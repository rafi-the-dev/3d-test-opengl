cmake_minimum_required(VERSION 3.10)
project(raudio)

add_library(raudio)
target_sources(raudio PRIVATE src/raudio.c)
target_include_directories(raudio PUBLIC include)

# raudio is legacy C99 code. Modern GCC/Clang default to C23, which turns
# `bool`/`true`/`false` into keywords (breaking raudio's own typedef) and
# promotes incompatible-pointer-type mismatches to hard errors. Pin the C
# standard and relax those diagnostics so the vendored source still builds.
if(NOT MSVC)
    set_target_properties(raudio PROPERTIES C_STANDARD 11 C_EXTENSIONS ON)
    target_compile_options(raudio PRIVATE
        -Wno-error=incompatible-pointer-types
        -Wno-incompatible-pointer-types)
endif()