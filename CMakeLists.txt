cmake_minimum_required(VERSION 3.1...3.17)

project(sen
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(sen
    src/ps.cc
    src/main.cc
)

if(MSVC)
    string(REGEX REPLACE "/GR|/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    target_compile_options(sen PRIVATE /FC /GR- /EHs-c- /W4 /WX)
else()
    target_compile_options(sen PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# find_package(LLVM REQUIRED CONFIG)
# target_include_directories(sen PRIVATE ${LLVM_INCLUDE_DIRS})
# target_compile_definitions(sen PRIVATE ${LLVM_DEFINITIONS})
# llvm_map_components_to_libnames(llvm_libs core native)
# target_link_libraries(sen PRIVATE ${llvm_libs})

set_target_properties(sen PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
