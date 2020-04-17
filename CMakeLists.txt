cmake_minimum_required(VERSION 3.1...3.17)

project(sen
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(sen
    src/main.cc
    src/lexer.cc
    src/intern.cc
)

if(MSVC)
    target_compile_options(sen PRIVATE /W4 /WX)
else()
    target_compile_options(sen PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

find_package(fmt CONFIG REQUIRED)
target_link_libraries(sen PRIVATE fmt::fmt)

set_target_properties(sen PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
