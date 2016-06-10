cmake_minimum_required(VERSION 3.5)
project(mksc)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_C_COMPILER                "/usr/bin/clang")
set(CMAKE_CXX_COMPILER              "/usr/bin/clang++")

set(CMAKE_AR      "/usr/bin/llvm-ar")
set(CMAKE_LINKER  "/usr/bin/llvm-ld")
set(CMAKE_NM      "/usr/bin/llvm-nm")
set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
set(CMAKE_RANLIB  "/usr/bin/llvm-ranlib")

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -O1 -Werror -Wall -std=gnu11 -fsanitize=address -fblocks -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS_DEBUG "-g")

add_definitions(-D_GNU_SOURCE=1)

include_directories(include)
include_directories(${CMAKE_BINARY_DIR})

find_package(RAGEL 6.9 REQUIRED)
ragel_target(lexer ${CMAKE_SOURCE_DIR}/src/lexer.rl ${CMAKE_BINARY_DIR}/lexer.c)

set(LEMON_SOURCE_FILE ${CMAKE_SOURCE_DIR}/deps/lemon.c)
set(LEMON_TEMPLATE ${CMAKE_SOURCE_DIR}/deps/lempar.c.tmpl)

find_package(LEMON 1.0 REQUIRED)
lemon_target(parser ${CMAKE_SOURCE_DIR}/src/parser.y ${CMAKE_BINARY_DIR})

set(SYNTAX_TEST_FILES
        tests/parsing/01-test-expression.syntax
        tests/parsing/02-test-assign.syntax
        tests/parsing/03-test-if-else-branch.syntax)

set(SOURCE_FILES
        src/main.c
        src/mks_node.c
        src/mks_token.c
        src/utils/file_utils.c
        src/utils/string_utils.c
        src/utils/memory_utils.c
        ${RAGEL_lexer_OUTPUTS}
        ${LEMON_parser_SOURCE_OUTPUT})

add_executable(mksc ${SOURCE_FILES})

## Compile and link blocksruntime
add_library(blocksruntime STATIC
        ${CMAKE_SOURCE_DIR}/deps/blocksruntime/BlocksRuntime/data.c
        ${CMAKE_SOURCE_DIR}/deps/blocksruntime/BlocksRuntime/runtime.c)
target_include_directories(blocksruntime PRIVATE ${CMAKE_SOURCE_DIR}/deps/blocksruntime/)
target_compile_options(blocksruntime PRIVATE -Wno-unused-function)

target_include_directories(mksc PRIVATE ${CMAKE_SOURCE_DIR}/deps/blocksruntime/BlocksRuntime)
target_link_libraries(mksc blocksruntime)