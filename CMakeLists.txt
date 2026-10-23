# SPDX-License-Identifier: GPL-3.0-only
cmake_minimum_required(VERSION 3.13)

project(inodeyou-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

find_library(TSK_LIB tsk)
if(NOT TSK_LIB)
    message(FATAL_ERROR "Sleuth Kit library not found. Please install libtsk-dev.")
endif()

option(INODEYOU_SANITIZE "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

function(inodeyou_harden target)
    target_compile_options(${target} PRIVATE
        -Wall -Wextra -Wpedantic
        -fstack-protector-strong
        -fstack-clash-protection)
    target_link_options(${target} PRIVATE -Wl,-z,relro,-z,now)
    set_target_properties(${target} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    if(CMAKE_BUILD_TYPE MATCHES "^(Release|RelWithDebInfo|MinSizeRel)$")
        target_compile_options(${target} PRIVATE -D_FORTIFY_SOURCE=2)
    endif()
    if(INODEYOU_SANITIZE)
        target_compile_options(${target} PRIVATE
            -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()

set(SHARED_SOURCES
    src/utils/inode_utils.cpp
    src/fs/fs_walk_path.cpp
    src/fs/populate_fs_inodes.cpp
    src/tsk/tsk_walk_path.cpp
    src/tsk/populate_tsk_inodes.cpp
)

add_executable(inodeyou src/main.cpp ${SHARED_SOURCES})
target_include_directories(inodeyou PRIVATE src src/fs src/tsk src/utils)
target_link_libraries(inodeyou ${TSK_LIB})
inodeyou_harden(inodeyou)

enable_testing()

add_executable(inventory_tests tests/inventory_tests.cpp ${SHARED_SOURCES})
target_include_directories(inventory_tests PRIVATE src src/fs src/tsk src/utils)
target_link_libraries(inventory_tests ${TSK_LIB})
inodeyou_harden(inventory_tests)
add_test(NAME inventories COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration.sh $<TARGET_FILE:inventory_tests>)

add_executable(traversal_tests tests/traversal_tests.cpp
    src/utils/inode_utils.cpp src/fs/fs_walk_path.cpp src/fs/populate_fs_inodes.cpp)
target_include_directories(traversal_tests PRIVATE src src/fs src/utils)
inodeyou_harden(traversal_tests)
add_test(NAME traversal COMMAND traversal_tests)
