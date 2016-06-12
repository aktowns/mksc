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

find_package(PkgConfig)
pkg_check_modules (UNWIND REQUIRED libunwind)

find_package(LLVM 3.9 REQUIRED all-targets analysis asmparser asmprinter bitreader bitwriter codegen core debuginfocodeview
        debuginfodwarf debuginfopdb globalisel instcombine ipa ipo instrumentation irreader linker lto mc mcdisassembler
        mcparser objcarcopts object option profiledata scalaropts selectiondag support tablegen target transformutils
        vectorize)

string(REGEX REPLACE "-DNDEBUG" "" LLVM_CFLAGS ${LLVM_CFLAGS})
string(REGEX REPLACE "-pedantic" "" LLVM_CFLAGS ${LLVM_CFLAGS})

set(DEBUGGING "-g -O1 -fsanitize=address -fsanitize=undefined -fsanitize-coverage=func -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS} "-Werror -Wall -std=gnu11 -fblocks ${LLVM_CFLAGS} ${UNWIND_CFLAGS} ${DEBUGGING}")
set(CMAKE_C_FLAGS_DEBUG "-g")

find_file(UTHashHeader uthash.h HINTS /usr/include/)
get_filename_component(UTHash_Include ${UTHashHeader} DIRECTORY)
include_directories(UTHash_Include)

add_definitions(-D_GNU_SOURCE=1)

include_directories(./include)
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
        tests/parsing/03-test-if-else-branch.syntax
        tests/parsing/04-test-do-block.syntax
        tests/parsing/05-test-function-call.syntax
        tests/parsing/06-test-function-declaration.syntax
        tests/parsing/07-test-import-statement.syntax
        tests/parsing/08-test-mode-export.syntax
        tests/parsing/09-test-strings.syntax
        tests/parsing/10-test-numbers.syntax
        tests/parsing/11-test-arrays.syntax
        tests/parsing/12-test-comments.syntax
        tests/parsing/13-test-tuples.syntax)

set(ENRICHMENT_TEST_FILES
        tests/inference/01-test-primitives.typed
        tests/inference/02-test-functions-curry.typed)

set(SOURCE_FILES
        src/main.c
        src/mks_node.c
        src/mks_token.c
        src/mks_type.c
        src/mks_enrichment.c
        src/codegen/module_codegen.c
        src/utils/file_utils.c
        src/utils/string_utils.c
        src/utils/memory_utils.c
        src/utils/node_list.c
        ${RAGEL_lexer_OUTPUTS}
        ${LEMON_parser_SOURCE_OUTPUT}

        src/lexer.rl
        src/parser.y

        include/mks_node.h
        include/mks_token.h
        include/mks_type.h
        include/mks_enrichment.h
        include/parser_exports.h
        include/codegen/module_codegen.h
        include/utils/ansi_colour.h
        include/utils/file_utils.h
        include/utils/log_utils.h
        include/utils/memory_utils.h
        include/utils/node_list.h
        include/utils/string_utils.h

        ${SYNTAX_TEST_FILES}
        ${ENRICHMENT_TEST_FILES})

add_executable(mksc ${SOURCE_FILES})
target_include_directories(mksc PUBLIC ${LLVM_INCLUDE_DIRS})
target_include_directories(mksc PUBLIC ${UNWIND_INCLUDE_DIRS})
target_link_libraries(mksc ${LLVM_LIBRARIES} ${LLVM_LDFLAGS})
target_link_libraries(mksc ${UNWIND_LIBRARIES})

## Compile and link blocksruntime
add_library(blocksruntime STATIC
        ${CMAKE_SOURCE_DIR}/deps/blocksruntime/BlocksRuntime/data.c
        ${CMAKE_SOURCE_DIR}/deps/blocksruntime/BlocksRuntime/runtime.c)
target_include_directories(blocksruntime PRIVATE ${CMAKE_SOURCE_DIR}/deps/blocksruntime/)
target_compile_options(blocksruntime PRIVATE -Wno-unused-function)

target_include_directories(mksc PRIVATE ${CMAKE_SOURCE_DIR}/deps/blocksruntime/BlocksRuntime)
target_link_libraries(mksc blocksruntime)
