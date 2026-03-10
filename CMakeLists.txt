cmake_minimum_required(VERSION 3.10)
project(thogcc VERSION 0.1.0 LANGUAGES CXX)

# Modern cpp please
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
   set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(
    ThogParser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
    # Generate parser.output always,
    # parse.trace only for debug builds
    OPTIONS -v $<$<CONFIG:Debug>:-t>
)

FLEX_TARGET(
    ThogLexer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.cpp
)

ADD_FLEX_BISON_DEPENDENCY(ThogLexer ThogParser)

file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(c_compiler
    ${SOURCES}
    ${BISON_ThogParser_OUTPUTS}
    ${FLEX_ThogLexer_OUTPUTS}
)

target_include_directories(c_compiler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_options(c_compiler PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()

find_program(LIT_EXECUTABLE lit)

if (LIT_EXECUTABLE)
    message(STATUS "lit found; adding tests")
    set(LIT_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    add_test(NAME lit_tests
        COMMAND ${LIT_EXECUTABLE}
            --xunit-xml-output ${CMAKE_BINARY_DIR}/lit_out.xml
            ${LIT_TEST_DIR}
    )
else()
    message(STATUS "lit not found; skipping tests")
endif()
